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
    void cFunctionWorking(int threadNum, int * val, int * wait);
    int NUM_MOVES
    int NUM_CHANNELS
    int NUM_ROWS
    int NUM_COLS
    int MAX_TURNS
    int BATCH_SIZE
    
cpdef selfPlayFull():
    model = load_model('./models/model.h5', custom_objects={'softmax_cross_entropy_with_logits': softmax_cross_entropy_with_logits})
    for temp in range(1):
        print('Start temp: ' + str(temp))
        for s in range(100):
            print('SelfPlaying: ' + str(s))
            selfPlay(model)
            print('Training: ' + str(s))
            Train.train(model)
            print('Trained: ' + str(s))
        print('End temp: ' + str(temp))

cpdef selfPlay(model):
    #model = load_model('./models/model.h5', custom_objects={'softmax_cross_entropy_with_logits': softmax_cross_entropy_with_logits})

    cdef np.ndarray[DTYPE_INT_t] gameState
    cdef np.ndarray[DTYPE_t] v
    cdef np.ndarray[DTYPE_t] p
    cdef np.ndarray[DTYPE_INT_t] isCReady
    cdef np.ndarray[DTYPE_INT_t] isModelReady
    cdef np.ndarray[DTYPE_INT_t] numTurns
    cdef np.ndarray[DTYPE_INT_t] gameStateOut
    cdef np.ndarray[DTYPE_t] vOut
    cdef np.ndarray[DTYPE_t] piOut
    cdef np.ndarray[DTYPE_INT_t] error
    cdef int * gameStatePointer
    cdef double * vPointer
    cdef double * pPointer
    cdef int * isCReadyPointer
    cdef int * isModelReadyPointer
    cdef int * numTurnsPPointer
    cdef int * gameStateOutPointer
    cdef double * vOutPointer
    cdef double * piOutPointer
    cdef int * errorPointer
    
    gameState = np.zeros((BATCH_SIZE*NUM_CHANNELS*NUM_ROWS*NUM_COLS), dtype=DTYPE_INT)
    v = np.zeros((BATCH_SIZE), dtype=DTYPE)
    p = np.zeros((BATCH_SIZE*NUM_MOVES), dtype=DTYPE)
    isCReady = np.zeros((1), dtype=DTYPE_INT)
    isModelReady = np.zeros((1), dtype=DTYPE_INT)
    numTurns = np.zeros((1), dtype=DTYPE_INT)
    gameStateOut = np.zeros((MAX_TURNS*NUM_CHANNELS*NUM_ROWS*NUM_COLS), dtype = DTYPE_INT)
    vOut = np.zeros((MAX_TURNS), dtype = DTYPE)
    piOut = np.zeros((MAX_TURNS*NUM_MOVES), dtype = DTYPE)
    error = np.zeros((1), dtype=DTYPE_INT)
    gameStatePointer = <int *> gameState.data
    vPointer = <double *> v.data
    pPointer = <double *> p.data
    isCReadyPointer = <int *> isCReady.data
    isModelReadyPointer = <int *> isModelReady.data
    numTurnsPointer = <int *> numTurns.data
    gameStateOutPointer = <int*>gameStateOut.data
    vOutPointer = <double*>vOut.data
    piOutPointer = <double*>piOut.data
    errorPointer = <int *> error.data
    
    threadA = threading.Thread(target=runSelfPlayC, args=(400, gameState, v, p, isCReady, isModelReady, numTurns, gameStateOut, vOut, piOut, error))
    
    threadA.start()
    
    cdef int i
    cdef int j
    cdef int k
    cdef np.ndarray[DTYPE_t] p_mid
    cdef np.ndarray[DTYPE_t] v_mid
    
    modelInput = np.zeros((BATCH_SIZE, NUM_ROWS, NUM_COLS, NUM_CHANNELS))
    while(threadA.is_alive()):
        if isCReadyPointer[0] == 1:
            modelInput = np.transpose(np.reshape(gameState, [BATCH_SIZE, NUM_CHANNELS, NUM_ROWS, NUM_COLS]), (0, 2, 3, 1))
            modelOut = model.predict(modelInput)
            v_model = modelOut[0].astype('d')
            p_model = modelOut[1].astype('d').flatten()
            v[:] = v_model[:, 0]
            p[:] = p_model[:]
            #p[:] = p_mid[:]
            isCReadyPointer[0] = 0
            isModelReadyPointer[0] = 1
    
    gameStateOutReshape = np.transpose(np.reshape(gameStateOut, [MAX_TURNS, NUM_CHANNELS, NUM_ROWS, NUM_COLS]), (0, 2, 3, 1))
    piOutReshape = np.reshape(piOut, [MAX_TURNS, NUM_MOVES])
    for i in range(numTurnsPointer[0]):
        savedState = SavedState(gameStateOutReshape[i], piOutReshape[i], vOut[i])
        savePath = "./positionsToBeProcessed/position-" + datetime.now().strftime("%d-%b-%Y-%H-%M-%S-%f")
        writeSavedState(savedState, savePath)
    
    if errorPointer[0] == 1:
        print('There was and Error, Investigate')
    #gameState[0] = 1
    #v[0] = 1
    #p[0] = 1
    #isCReady[0] = 1
    #isModelReady[0] = 1
    #error[0] = 1
    threadA.join()
    
    
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
