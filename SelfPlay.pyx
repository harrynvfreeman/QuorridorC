import numpy as np
cimport numpy as np
from keras.models import load_model
from loss import softmax_cross_entropy_with_logits
from time import sleep
from numpy cimport ndarray
import threading
from libc.stdlib cimport malloc, free
import random
from SavedState import SavedState, writeSavedState
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
            readMove = int(input("What is your age? "))
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
    while True:
        print('Start temp: ' + str(temp))
        for s in range(12):
            print('SelfPlaying: ' + str(s))
            selfPlay(model)
            print('Done: ' + str(s))
        print('End temp: ' + str(temp))
        Train.train(model)
        temp = temp + 1

cpdef selfPlay(model):
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
    errorPointer0 = <int *> error0.data
    
    ######################################
    
    cdef np.ndarray[DTYPE_INT_t] gameState1
    cdef np.ndarray[DTYPE_t] v1
    cdef np.ndarray[DTYPE_t] p1
    cdef np.ndarray[DTYPE_INT_t] isCReady1
    cdef np.ndarray[DTYPE_INT_t] isModelReady1
    cdef np.ndarray[DTYPE_INT_t] numTurns1
    cdef np.ndarray[DTYPE_INT_t] gameStateOut1
    cdef np.ndarray[DTYPE_t] vOut1
    cdef np.ndarray[DTYPE_t] piOut1
    cdef np.ndarray[DTYPE_INT_t] error1
    cdef int * gameStatePointer1
    cdef double * vPointer1
    cdef double * pPointer1
    cdef int * isCReadyPointer1
    cdef int * isModelReadyPointer1
    cdef int * numTurnsPPointer1
    cdef int * gameStateOutPointer1
    cdef double * vOutPointer1
    cdef double * piOutPointer1
    cdef int * errorPointer1
    
    gameState1 = np.zeros((BATCH_SIZE*NUM_CHANNELS*NUM_ROWS*NUM_COLS), dtype=DTYPE_INT)
    v1 = np.zeros((BATCH_SIZE), dtype=DTYPE)
    p1 = np.zeros((BATCH_SIZE*NUM_MOVES), dtype=DTYPE)
    isCReady1 = np.zeros((1), dtype=DTYPE_INT)
    isModelReady1 = np.zeros((1), dtype=DTYPE_INT)
    numTurns1 = np.zeros((1), dtype=DTYPE_INT)
    gameStateOut1 = np.zeros((MAX_TURNS*NUM_CHANNELS*NUM_ROWS*NUM_COLS), dtype = DTYPE_INT)
    vOut1 = np.zeros((MAX_TURNS), dtype = DTYPE)
    piOut1 = np.zeros((MAX_TURNS*NUM_MOVES), dtype = DTYPE)
    error1 = np.zeros((1), dtype=DTYPE_INT)
    gameStatePointer1 = <int *> gameState1.data
    vPointer1 = <double *> v1.data
    pPointer1 = <double *> p1.data
    isCReadyPointer1 = <int *> isCReady1.data
    isModelReadyPointer1 = <int *> isModelReady1.data
    numTurnsPointer1 = <int *> numTurns1.data
    gameStateOutPointer1 = <int*>gameStateOut1.data
    vOutPointer1 = <double*>vOut1.data
    piOutPointer1 = <double*>piOut1.data
    errorPointer1 = <int *> error1.data
    
    ########################################################################
    
    cdef np.ndarray[DTYPE_INT_t] gameState2
    cdef np.ndarray[DTYPE_t] v2
    cdef np.ndarray[DTYPE_t] p2
    cdef np.ndarray[DTYPE_INT_t] isCReady2
    cdef np.ndarray[DTYPE_INT_t] isModelReady2
    cdef np.ndarray[DTYPE_INT_t] numTurns2
    cdef np.ndarray[DTYPE_INT_t] gameStateOut2
    cdef np.ndarray[DTYPE_t] vOut2
    cdef np.ndarray[DTYPE_t] piOut2
    cdef np.ndarray[DTYPE_INT_t] error2
    cdef int * gameStatePointer2
    cdef double * vPointer2
    cdef double * pPointer2
    cdef int * isCReadyPointer2
    cdef int * isModelReadyPointer2
    cdef int * numTurnsPPointer2
    cdef int * gameStateOutPointer2
    cdef double * vOutPointer2
    cdef double * piOutPointer2
    cdef int * errorPointer2
    
    gameState2 = np.zeros((BATCH_SIZE*NUM_CHANNELS*NUM_ROWS*NUM_COLS), dtype=DTYPE_INT)
    v2 = np.zeros((BATCH_SIZE), dtype=DTYPE)
    p2 = np.zeros((BATCH_SIZE*NUM_MOVES), dtype=DTYPE)
    isCReady2 = np.zeros((1), dtype=DTYPE_INT)
    isModelReady2 = np.zeros((1), dtype=DTYPE_INT)
    numTurns2 = np.zeros((1), dtype=DTYPE_INT)
    gameStateOut2 = np.zeros((MAX_TURNS*NUM_CHANNELS*NUM_ROWS*NUM_COLS), dtype = DTYPE_INT)
    vOut2 = np.zeros((MAX_TURNS), dtype = DTYPE)
    piOut2 = np.zeros((MAX_TURNS*NUM_MOVES), dtype = DTYPE)
    error2 = np.zeros((1), dtype=DTYPE_INT)
    gameStatePointer2 = <int *> gameState2.data
    vPointer2 = <double *> v2.data
    pPointer2 = <double *> p2.data
    isCReadyPointer2 = <int *> isCReady2.data
    isModelReadyPointer2 = <int *> isModelReady2.data
    numTurnsPointer2 = <int *> numTurns2.data
    gameStateOutPointer2 = <int*>gameStateOut2.data
    vOutPointer2 = <double*>vOut2.data
    piOutPointer2 = <double*>piOut2.data
    errorPointer2 = <int *> error2.data
    
    ###############################################
    
    cdef np.ndarray[DTYPE_INT_t] gameState3
    cdef np.ndarray[DTYPE_t] v3
    cdef np.ndarray[DTYPE_t] p3
    cdef np.ndarray[DTYPE_INT_t] isCReady3
    cdef np.ndarray[DTYPE_INT_t] isModelReady3
    cdef np.ndarray[DTYPE_INT_t] numTurns3
    cdef np.ndarray[DTYPE_INT_t] gameStateOut3
    cdef np.ndarray[DTYPE_t] vOut3
    cdef np.ndarray[DTYPE_t] piOut3
    cdef np.ndarray[DTYPE_INT_t] error3
    cdef int * gameStatePointer3
    cdef double * vPointer3
    cdef double * pPointer3
    cdef int * isCReadyPointer3
    cdef int * isModelReadyPointer3
    cdef int * numTurnsPPointer3
    cdef int * gameStateOutPointer3
    cdef double * vOutPointer3
    cdef double * piOutPointer3
    cdef int * errorPointer3
    
    gameState3 = np.zeros((BATCH_SIZE*NUM_CHANNELS*NUM_ROWS*NUM_COLS), dtype=DTYPE_INT)
    v3 = np.zeros((BATCH_SIZE), dtype=DTYPE)
    p3 = np.zeros((BATCH_SIZE*NUM_MOVES), dtype=DTYPE)
    isCReady3 = np.zeros((1), dtype=DTYPE_INT)
    isModelReady3 = np.zeros((1), dtype=DTYPE_INT)
    numTurns3 = np.zeros((1), dtype=DTYPE_INT)
    gameStateOut3 = np.zeros((MAX_TURNS*NUM_CHANNELS*NUM_ROWS*NUM_COLS), dtype = DTYPE_INT)
    vOut3 = np.zeros((MAX_TURNS), dtype = DTYPE)
    piOut3 = np.zeros((MAX_TURNS*NUM_MOVES), dtype = DTYPE)
    error3 = np.zeros((1), dtype=DTYPE_INT)
    gameStatePointer3 = <int *> gameState3.data
    vPointer3 = <double *> v3.data
    pPointer3 = <double *> p3.data
    isCReadyPointer3 = <int *> isCReady3.data
    isModelReadyPointer3 = <int *> isModelReady3.data
    numTurnsPointer3 = <int *> numTurns3.data
    gameStateOutPointer3 = <int*>gameStateOut3.data
    vOutPointer3 = <double*>vOut3.data
    piOutPointer3 = <double*>piOut3.data
    errorPointer3 = <int *> error3.data
    
    ##########################################################3
    
    thread0 = threading.Thread(target=runSelfPlayC, args=(200, gameState0, v0, p0, isCReady0, isModelReady0, numTurns0, gameStateOut0, vOut0, piOut0, error0))
    thread1 = threading.Thread(target=runSelfPlayC, args=(400, gameState1, v1, p1, isCReady1, isModelReady1, numTurns1, gameStateOut1, vOut1, piOut1, error1))
    thread2 = threading.Thread(target=runSelfPlayC, args=(400, gameState2, v2, p2, isCReady2, isModelReady2, numTurns2, gameStateOut2, vOut2, piOut2, error2))
    thread3 = threading.Thread(target=runSelfPlayC, args=(400, gameState3, v3, p3, isCReady3, isModelReady3, numTurns3, gameStateOut3, vOut3, piOut3, error3))
    
    thread0.start()
    #thread1.start()
    #thread2.start()
    #thread3.start()
    
    cdef int i
    cdef int j
    cdef int k
    cdef np.ndarray[DTYPE_t] p_mid
    cdef np.ndarray[DTYPE_t] v_mid
    
    modelInput = np.zeros((BATCH_SIZE, NUM_ROWS, NUM_COLS, NUM_CHANNELS))
    while(thread0.is_alive() or thread1.is_alive() or thread2.is_alive() or thread3.is_alive()):
        if isCReadyPointer0[0] == 1:
            modelInput = np.transpose(np.reshape(gameState0, [BATCH_SIZE, NUM_CHANNELS, NUM_ROWS, NUM_COLS]), (0, 2, 3, 1))
            modelOut = model.predict(modelInput)
            v_model = modelOut[0].astype('d')
            p_model = modelOut[1].astype('d').flatten()
            v0[:] = v_model[:, 0]
            p0[:] = p_model[:]
            isCReadyPointer0[0] = 0
            isModelReadyPointer0[0] = 1
        #############
        if isCReadyPointer1[0] == 1:
            modelInput = np.transpose(np.reshape(gameState1, [BATCH_SIZE, NUM_CHANNELS, NUM_ROWS, NUM_COLS]), (0, 2, 3, 1))
            modelOut = model.predict(modelInput)
            v_model = modelOut[0].astype('d')
            p_model = modelOut[1].astype('d').flatten()
            v1[:] = v_model[:, 0]
            p1[:] = p_model[:]
            isCReadyPointer1[0] = 0
            isModelReadyPointer1[0] = 1
        ################
        if isCReadyPointer2[0] == 1:
            modelInput = np.transpose(np.reshape(gameState2, [BATCH_SIZE, NUM_CHANNELS, NUM_ROWS, NUM_COLS]), (0, 2, 3, 1))
            modelOut = model.predict(modelInput)
            v_model = modelOut[0].astype('d')
            p_model = modelOut[1].astype('d').flatten()
            v2[:] = v_model[:, 0]
            p2[:] = p_model[:]
            isCReadyPointer2[0] = 0
            isModelReadyPointer2[0] = 1
        #################
        if isCReadyPointer3[0] == 1:
            modelInput = np.transpose(np.reshape(gameState3, [BATCH_SIZE, NUM_CHANNELS, NUM_ROWS, NUM_COLS]), (0, 2, 3, 1))
            modelOut = model.predict(modelInput)
            v_model = modelOut[0].astype('d')
            p_model = modelOut[1].astype('d').flatten()
            v3[:] = v_model[:, 0]
            p3[:] = p_model[:]
            isCReadyPointer3[0] = 0
            isModelReadyPointer3[0] = 1
        #################
    
    #gameStateOutReshape = np.transpose(np.reshape(gameStateOut0, [MAX_TURNS, NUM_CHANNELS, NUM_ROWS, NUM_COLS]), (0, 2, 3, 1))
    #piOutReshape = np.reshape(piOut0, [MAX_TURNS, NUM_MOVES])
    #for i in range(numTurnsPointer0[0]):
        #savedState = SavedState(gameStateOutReshape[i], piOutReshape[i], vOut0[i])
        #savePath = "./positionsToBeProcessed/position-" + datetime.now().strftime("%d-%b-%Y-%H-%M-%S-%f")
        #writeSavedState(savedState, savePath)
    ###########################
    #gameStateOutReshape = np.transpose(np.reshape(gameStateOut1, [MAX_TURNS, NUM_CHANNELS, NUM_ROWS, NUM_COLS]), (0, 2, 3, 1))
    #piOutReshape = np.reshape(piOut1, [MAX_TURNS, NUM_MOVES])
    #for i in range(numTurnsPointer1[0]):
        #savedState = SavedState(gameStateOutReshape[i], piOutReshape[i], vOut1[i])
        #savePath = "./positionsToBeProcessed/position-" + datetime.now().strftime("%d-%b-%Y-%H-%M-%S-%f")
        #writeSavedState(savedState, savePath)
    ###########################
    #gameStateOutReshape = np.transpose(np.reshape(gameStateOut2, [MAX_TURNS, NUM_CHANNELS, NUM_ROWS, NUM_COLS]), (0, 2, 3, 1))
    #piOutReshape = np.reshape(piOut2, [MAX_TURNS, NUM_MOVES])
    #for i in range(numTurnsPointer2[0]):
        #savedState = SavedState(gameStateOutReshape[i], piOutReshape[i], vOut2[i])
        #savePath = "./positionsToBeProcessed/position-" + datetime.now().strftime("%d-%b-%Y-%H-%M-%S-%f")
        #writeSavedState(savedState, savePath)
    #############################
    #gameStateOutReshape = np.transpose(np.reshape(gameStateOut3, [MAX_TURNS, NUM_CHANNELS, NUM_ROWS, NUM_COLS]), (0, 2, 3, 1))
    #piOutReshape = np.reshape(piOut3, [MAX_TURNS, NUM_MOVES])
    #for i in range(numTurnsPointer3[0]):
        #savedState = SavedState(gameStateOutReshape[i], piOutReshape[i], vOut3[i])
        #savePath = "./positionsToBeProcessed/position-" + datetime.now().strftime("%d-%b-%Y-%H-%M-%S-%f")
        #writeSavedState(savedState, savePath)
    ###########################
    
    if errorPointer0[0] == 1 or errorPointer1[0] == 1 or errorPointer2[0] == 1 or errorPointer3[0] == 1:
        print('There was and Error, Investigate')
    #gameState[0] = 1
    #v[0] = 1
    #p[0] = 1
    #isCReady[0] = 1
    #isModelReady[0] = 1
    #error[0] = 1
    thread0.join()
    #thread1.join()
    #thread2.join()
    #thread3.join()
    
    
cdef runSelfPlayC(int numSimulations, np.ndarray[DTYPE_INT_t] gameState, np.ndarray[DTYPE_t] v, np.ndarray[DTYPE_t] p,
np.ndarray[DTYPE_INT_t] isCReady, np.ndarray[DTYPE_INT_t] isModelReady,
np.ndarray[DTYPE_INT_t] numTurns, np.ndarray[DTYPE_INT_t] gameStateOut, np.ndarray[DTYPE_t] vOut, np.ndarray[DTYPE_t] piOut,
np.ndarray[DTYPE_INT_t] error):
    selfPlayCython(numSimulations, <int *> gameState.data, <double *> v.data, <double *> p.data,
    <int *> isCReady.data, <int *> isModelReady.data,
    <int *> numTurns.data, <int *> gameStateOut.data, <double *> vOut.data, <double *> piOut.data,
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
