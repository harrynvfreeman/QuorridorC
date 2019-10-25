import numpy as np
cimport numpy as np
from keras.models import load_model
from loss import softmax_cross_entropy_with_logits
from time import sleep
from numpy cimport ndarray
import threading
from libc.stdlib cimport malloc, free
import random
from SavedState import SavedState, writeSavedState, readSavedState
from datetime import datetime
import Train

#DTYPE = np.float32
DTYPE = np.dtype('d')
ctypedef double DTYPE_t

#DTYPE_INT = np.int32
DTYPE_INT = np.dtype('i4')
#ctypedef np.int32_t DTYPE_INT_t
ctypedef int DTYPE_INT_t

cdef extern from "Quorridor.h":
    void selfPlayCython(int numSimulations, int * gameState, double * v, double * p,
    int * isCReady, int * isModelReady, 
    int * numTurns, int * gameStateOut, double * vOut, double * piOut,
    double * pRChoice, int * indRChoice, int * rChoiceReadyC, int * rChoiceReadyModel,
    int * error)
    void playMatchCython(int numSimulations, int * gameState, double * v, double * p, 
    int * isCReady, int * isModelReady, 
    int * isCReadyForHuman, int * isHumanReady, int * humanMove,
    int * error)
    void cFunctionWorking(int threadNum, int * val, int * wait);
    int NUM_MOVES
    int NUM_CHANNELS
    int NUM_ROWS
    int NUM_COLS
    int MAX_TURNS
    int BATCH_SIZE
    
cpdef playMatch():
    model = load_model('./models/model.h5', custom_objects={'softmax_cross_entropy_with_logits': softmax_cross_entropy_with_logits})
    
    cdef np.ndarray[DTYPE_INT_t] gameState0
    cdef np.ndarray[DTYPE_t] v0
    cdef np.ndarray[DTYPE_t] p0
    cdef np.ndarray[DTYPE_INT_t] isCReady0
    cdef np.ndarray[DTYPE_INT_t] isModelReady0
    cdef np.ndarray[DTYPE_INT_t] isCReadyForHuman0
    cdef np.ndarray[DTYPE_INT_t] isHumanReady0
    cdef np.ndarray[DTYPE_INT_t] humanMove0
    cdef np.ndarray[DTYPE_INT_t] error0
    cdef int * gameStatePointer0
    cdef double * vPointer0
    cdef double * pPointer0
    cdef int * isCReadyPointer0
    cdef int * isModelReadyPointer0
    cdef int * isCReadyForHumanPointer0
    cdef int * isHumanReadyPointer0
    cdef int * humanMovePointer0
    cdef int * errorPointer0
    
    gameState0 = np.zeros((BATCH_SIZE*NUM_CHANNELS*NUM_ROWS*NUM_COLS), dtype=DTYPE_INT)
    v0 = np.zeros((BATCH_SIZE), dtype=DTYPE)
    p0 = np.zeros((BATCH_SIZE*NUM_MOVES), dtype=DTYPE)
    isCReady0 = np.zeros((1), dtype=DTYPE_INT)
    isModelReady0 = np.zeros((1), dtype=DTYPE_INT)
    isCReadyForHuman0 = np.zeros((1), dtype = DTYPE_INT)
    isHumanReady0 = np.zeros((1), dtype = DTYPE_INT)
    humanMove0 = np.zeros((1), dtype = DTYPE_INT)
    error0 = np.zeros((1), dtype=DTYPE_INT)
    gameStatePointer0 = <int *> gameState0.data
    vPointer0 = <double *> v0.data
    pPointer0 = <double *> p0.data
    isCReadyPointer0 = <int *> isCReady0.data
    isModelReadyPointer0 = <int *> isModelReady0.data
    isCReadyForHumanPointer0 = <int*>isCReadyForHuman0.data
    isHumanReadyPointer0 = <int*>isHumanReady0.data
    humanMovePointer0 = <int*>humanMove0.data
    errorPointer0 = <int *> error0.data
    
    thread0 = threading.Thread(target=runPlayMatchC, args=(400, gameState0, v0, p0, isCReady0, isModelReady0, isCReadyForHuman0, isHumanReady0, humanMove0, error0))
    thread0.start()
    
    cdef int readMove
    
    modelInput = np.zeros((BATCH_SIZE, NUM_ROWS, NUM_COLS, NUM_CHANNELS))
    while(thread0.is_alive()):
        if isCReadyPointer0[0] == 1:
            modelInput = np.transpose(np.reshape(gameState0, [BATCH_SIZE, NUM_CHANNELS, NUM_ROWS, NUM_COLS]), (0, 2, 3, 1))
            modelOut = model.predict(modelInput)
            v_model = modelOut[0].astype('d')
            p_model = modelOut[1].astype('d').flatten()
            v0[:] = v_model[:, 0]
            p0[:] = p_model[:]
            isCReadyPointer0[0] = 0
            isModelReadyPointer0[0] = 1
        if isCReadyForHumanPointer0[0] == 1:
            readMove = int(input("What is your move? "))
            humanMovePointer0[0] = readMove
            isCReadyForHumanPointer0[0] = 0
            isHumanReadyPointer0[0] = 1
            
    thread0.join()

cdef runPlayMatchC(int numSimulations, np.ndarray[DTYPE_INT_t] gameState, np.ndarray[DTYPE_t] v, np.ndarray[DTYPE_t] p,
np.ndarray[DTYPE_INT_t] isCReady, np.ndarray[DTYPE_INT_t] isModelReady,
np.ndarray[DTYPE_INT_t] isCReadyForHuman, np.ndarray[DTYPE_INT_t] isHumanReady, np.ndarray[DTYPE_INT_t] humanMove,
np.ndarray[DTYPE_INT_t] error):
    playMatchCython(numSimulations, <int *> gameState.data, <double *> v.data, <double *> p.data,
    <int *> isCReady.data, <int *> isModelReady.data,
    <int *> isCReadyForHuman.data, <int *> isHumanReady.data, <int *> humanMove.data,
    <int *> error.data)
###########################################################################
cpdef selfPlayFull():
    model = load_model('./models/model.h5', custom_objects={'softmax_cross_entropy_with_logits': softmax_cross_entropy_with_logits})
    temp = 0
    cdef int gameNumber
    while True:
        gameNumber = 0
        print('Start temp: ' + str(temp))
        for s in range(200):
            print('SelfPlaying: ' + str(s))
            selfPlay(model, gameNumber)
            gameNumber = gameNumber + 1
            print('Done: ' + str(s))
        print('End temp: ' + str(temp))
        Train.train(model)
        temp = temp + 1

cpdef selfPlay(model, int gameNumber):
    #model = load_model('./models/model.h5', custom_objects={'softmax_cross_entropy_with_logits': softmax_cross_entropy_with_logits})

    cdef np.ndarray[DTYPE_INT_t] gameState0
    cdef np.ndarray[DTYPE_t] v0
    cdef np.ndarray[DTYPE_t] p0
    cdef np.ndarray[DTYPE_INT_t] isCReady0
    cdef np.ndarray[DTYPE_INT_t] isModelReady0
    cdef np.ndarray[DTYPE_INT_t] numTurns0
    cdef np.ndarray[DTYPE_INT_t] gameStateOut0
    cdef np.ndarray[DTYPE_t] vOut0
    cdef np.ndarray[DTYPE_t] piOut0
    cdef np.ndarray[DTYPE_t] pRChoice0
    cdef np.ndarray[DTYPE_INT_t] indRChoice0
    cdef np.ndarray[DTYPE_INT_t] rChoiceReadyC0
    cdef np.ndarray[DTYPE_INT_t] rChoiceReadyModel0
    cdef np.ndarray[DTYPE_INT_t] error0
    cdef int * gameStatePointer0
    cdef double * vPointer0
    cdef double * pPointer0
    cdef int * isCReadyPointer0
    cdef int * isModelReadyPointer0
    cdef int * numTurnsPPointer0
    cdef int * gameStateOutPointer0
    cdef double * vOutPointer0
    cdef double * piOutPointer0
    cdef double * pRChoicePointer0
    cdef int * indRChoicePointer0
    cdef int * rChoiceReadyCPointer0
    cdef int * rChoiceReadyModelPointer0
    cdef int * errorPointer0
    
    gameState0 = np.zeros((BATCH_SIZE*NUM_CHANNELS*NUM_ROWS*NUM_COLS), dtype=DTYPE_INT)
    v0 = np.zeros((BATCH_SIZE), dtype=DTYPE)
    p0 = np.zeros((BATCH_SIZE*NUM_MOVES), dtype=DTYPE)
    isCReady0 = np.zeros((1), dtype=DTYPE_INT)
    isModelReady0 = np.zeros((1), dtype=DTYPE_INT)
    numTurns0 = np.zeros((1), dtype=DTYPE_INT)
    gameStateOut0 = np.zeros((MAX_TURNS*NUM_CHANNELS*NUM_ROWS*NUM_COLS), dtype = DTYPE_INT)
    vOut0 = np.zeros((MAX_TURNS), dtype = DTYPE)
    piOut0 = np.zeros((MAX_TURNS*NUM_MOVES), dtype = DTYPE)
    pRChoice0 = np.zeros((NUM_MOVES), dtype = DTYPE)
    indRChoice0 = np.zeros((1), dtype = DTYPE_INT)
    rChoiceReadyC0 = np.zeros((1), dtype = DTYPE_INT)
    rChoiceReadyModel0 = np.zeros((1), dtype = DTYPE_INT)
    error0 = np.zeros((1), dtype=DTYPE_INT)
    gameStatePointer0 = <int *> gameState0.data
    vPointer0 = <double *> v0.data
    pPointer0 = <double *> p0.data
    isCReadyPointer0 = <int *> isCReady0.data
    isModelReadyPointer0 = <int *> isModelReady0.data
    numTurnsPointer0 = <int *> numTurns0.data
    gameStateOutPointer0 = <int*>gameStateOut0.data
    vOutPointer0 = <double*>vOut0.data
    piOutPointer0 = <double*>piOut0.data
    pRChoicePointer0 = <double*>pRChoice0.data
    indRChoicePointer0 = <int*>indRChoice0.data
    rChoiceReadyCPointer0 = <int*>rChoiceReadyC0.data
    rChoiceReadyModelPointer0 = <int*>rChoiceReadyModel0.data
    errorPointer0 = <int *> error0.data
    
    thread0 = threading.Thread(target=runSelfPlayC, args=(400, gameState0, v0, p0, isCReady0, isModelReady0, numTurns0, gameStateOut0, vOut0, piOut0, pRChoice0, indRChoice0, rChoiceReadyC0, rChoiceReadyModel0, error0))
    
    thread0.start()
    
    cdef int i
    cdef int j
    cdef int k
    cdef np.ndarray[DTYPE_t] p_mid
    cdef np.ndarray[DTYPE_t] v_mid
    
    modelInput = np.zeros((BATCH_SIZE, NUM_ROWS, NUM_COLS, NUM_CHANNELS))
    while(thread0.is_alive()):
        if isCReadyPointer0[0] == 1:
            modelInput = np.transpose(np.reshape(gameState0, [BATCH_SIZE, NUM_CHANNELS, NUM_ROWS, NUM_COLS]), (0, 2, 3, 1))
            modelOut = model.predict(modelInput)
            v_model = modelOut[0].astype('d')
            #v_model = 2*np.random.rand(BATCH_SIZE).astype('d') - 1
            p_model = modelOut[1].astype('d').flatten()
            #p_model = np.random.rand(BATCH_SIZE*NUM_MOVES).astype('d')
            v0[:] = v_model[:, 0]
            #v0[:] = v_model[:]
            p0[:] = p_model[:]
            isCReadyPointer0[0] = 0
            isModelReadyPointer0[0] = 1
        if rChoiceReadyCPointer0[0] == 1:
            choice = np.random.choice(NUM_MOVES,  p=pRChoice0)
            indRChoicePointer0[0] = choice
            rChoiceReadyCPointer0[0] = 0
            rChoiceReadyModelPointer0[0] = 1
    
    gameStateOutReshape = np.transpose(np.reshape(gameStateOut0, [MAX_TURNS, NUM_CHANNELS, NUM_ROWS, NUM_COLS]), (0, 2, 3, 1))
    piOutReshape = np.reshape(piOut0, [MAX_TURNS, NUM_MOVES])
    if vOut0[i] == 0:
        drawDescriptor = readSavedState("./models/draw")
        drawDescriptor.version = drawDescriptor.version + 1
        writeSavedState(drawDescriptor, "./models/draw")
    else:
        winDescriptor = readSavedState("./models/win")
        winDescriptor.version = winDescriptor.version + 1
        writeSavedState(winDescriptor, "./models/win")
    for i in range(numTurnsPointer0[0]):
        savedState = SavedState(gameStateOutReshape[i], piOutReshape[i], vOut0[i])
        savePath = "./positionsToBeProcessed/position-" + str(gameNumber) + "-" + datetime.now().strftime("%d-%b-%Y-%H-%M-%S-%f")
        writeSavedState(savedState, savePath)
    
    if errorPointer0[0] == 1:
        print('There was and Error, Investigate')
        
    thread0.join()
    
    
cdef runSelfPlayC(int numSimulations, np.ndarray[DTYPE_INT_t] gameState, np.ndarray[DTYPE_t] v, np.ndarray[DTYPE_t] p,
np.ndarray[DTYPE_INT_t] isCReady, np.ndarray[DTYPE_INT_t] isModelReady,
np.ndarray[DTYPE_INT_t] numTurns, np.ndarray[DTYPE_INT_t] gameStateOut, np.ndarray[DTYPE_t] vOut, np.ndarray[DTYPE_t] piOut,
np.ndarray[DTYPE_t] pRChoice, np.ndarray[DTYPE_INT_t] indRChoice, np.ndarray[DTYPE_INT_t] rChoiceReadyC, np.ndarray[DTYPE_INT_t] rChoiceReadyModel,
np.ndarray[DTYPE_INT_t] error):
    selfPlayCython(numSimulations, <int *> gameState.data, <double *> v.data, <double *> p.data,
    <int *> isCReady.data, <int *> isModelReady.data,
    <int *> numTurns.data, <int *> gameStateOut.data, <double *> vOut.data, <double *> piOut.data,
    <double *> pRChoice.data, <int *> indRChoice.data, <int *> rChoiceReadyC.data, <int *> rChoiceReadyModel.data,
    <int *> error.data)

cpdef testA():
    cdef np.ndarray[DTYPE_INT_t] val
    cdef np.ndarray[DTYPE_INT_t] wait
    cdef int * valPointer
    cdef int * waitPointer
    
    val = np.zeros((1), dtype=DTYPE_INT)
    wait = np.zeros((1), dtype=DTYPE_INT)
    valPointer = <int *> val.data
    waitPointer = <int *> wait.data
    
    threadA = threading.Thread(target=testCCode, args=(0, val, wait,));
    
    threadA.start()
    
    while valPointer[0] < 10:
        print('Python Waiting: ' + str(valPointer[0]))
        sleep(1)
    
    waitPointer[0] = 1
    
    threadA.join()
    print('Done')

cpdef test():
    cdef np.ndarray[DTYPE_INT_t] valA
    cdef np.ndarray[DTYPE_INT_t] waitA
    cdef int * valAPointer
    cdef int * waitAPointer
    cdef np.ndarray[DTYPE_INT_t] valB
    cdef np.ndarray[DTYPE_INT_t] waitB
    cdef int * valBPointer
    cdef int * waitBPointer
        
    valA = np.zeros((1), dtype=DTYPE_INT)
    waitA = np.zeros((1), dtype=DTYPE_INT)
    valAPointer = <int *> valA.data
    waitAPointer = <int *> waitA.data
    valB = np.zeros((1), dtype=DTYPE_INT)
    waitB = np.zeros((1), dtype=DTYPE_INT)
    valBPointer = <int *> valB.data
    waitBPointer = <int *> waitB.data
    
    threadA = threading.Thread(target=testCCode, args=(0, valA, waitA,));
    threadB = threading.Thread(target=testCCode, args=(1, valB, waitB,));
    
    threadA.start()
    #sleep(1)
    threadB.start()
    
    while valAPointer[0] < 10 and valBPointer[0] < 10:
        print('Python Waiting: val0 is ' + str(valAPointer[0]) + ' and val1 is ' + str(valBPointer[0]))
        sleep(1)
    
    waitAPointer[0] = 1
    waitBPointer[0] = 1
    
    threadA.join()
    threadB.join()
    print('Done')
    
cdef testCCode(int numThread, np.ndarray[DTYPE_INT_t] val, np.ndarray[DTYPE_INT_t] wait):
    #with nogil:
    cFunctionWorking(numThread, <int *> val.data, <int *> wait.data)

#model = load_model('./models/' + modelName + '.h5', custom_objects={'softmax_cross_entropy_with_logits': softmax_cross_entropy_with_logits})
#model.save('./depModels/' + modelName + '-' + datetime.now().strftime("%d-%b-%Y-%H-%M-%S-%f") + '.h5')
