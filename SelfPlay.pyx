import numpy as np
cimport numpy as np
#from keras.models import load_model
#loss import softmax_cross_entropy_with_logits
from time import sleep
from numpy cimport ndarray
import threading
from libc.stdlib cimport malloc, free
#from cython.parallel import parallel, prange
import random

#DTYPE = np.float32
DTYPE = np.dtype('d')
ctypedef double DTYPE_t

DTYPE_INT = np.int32
ctypedef np.int32_t DTYPE_INT_t

cdef extern from "Quorridor.h":
    void selfPlayCython(int numSimulations, int * gameState, double * v, double * p, int * isCReady, int * isModelReady, int * error);
    void cFunctionWorking(int threadNum, int * val, int * wait);
    int NUM_MOVES
    int NUM_CHANNELS
    int NUM_ROWS
    int NUM_COLS
    
cpdef selfPlay():
    cdef np.ndarray[DTYPE_INT_t] gameState
    cdef np.ndarray[DTYPE_t] v
    cdef np.ndarray[DTYPE_t] p
    cdef np.ndarray[DTYPE_INT_t] isCReady
    cdef np.ndarray[DTYPE_INT_t] isModelReady
    cdef np.ndarray[DTYPE_INT_t] error
    cdef int * gameStatePointer
    cdef double * vPointer
    cdef double * pPointer
    cdef int * isCReadyPointer
    cdef int * isModelReadyPointer
    cdef int * errorPointer
    
    gameState = np.zeros((NUM_CHANNELS*NUM_ROWS*NUM_COLS), dtype=DTYPE_INT)
    v = np.zeros((1), dtype=DTYPE)
    p = np.zeros((NUM_MOVES), dtype=DTYPE)
    isCReady = np.zeros((1), dtype=DTYPE_INT)
    isModelReady = np.zeros((1), dtype=DTYPE_INT)
    error = np.zeros((1), dtype=DTYPE_INT)
    gameStatePointer = <int *> gameState.data
    vPointer = <double *> v.data
    pPointer = <double *> p.data
    isCReadyPointer = <int *> isCReady.data
    isModelReadyPointer = <int *> isModelReady.data
    errorPointer = <int *> error.data
    
    threadA = threading.Thread(target=runSelfPlayC, args=(400, gameState, v, p, isCReady, isModelReady, error))
    
    threadA.start()
    
    cdef int i
    cdef float sum
    
    while(threadA.isAlive()):
        #print('Python am here and isModelReady is: ' + str(isModelReadyPointer[0]) )
        if isCReadyPointer[0] == 1:
            vPointer[0] = 3
            sum = 0
            for i in range(NUM_MOVES):
                pPointer[i] = 1/NUM_MOVES
            isCReadyPointer[0] = 0
            isModelReadyPointer[0] = 1
        #sleep(0.001)
    
    print('ThreadA died')
    gameState[0] = 1
    v[0] = 1
    p[0] = 1
    isCReady[0] = 1
    isModelReady[0] = 1
    error[0] = 1
    print('Python almost done')
    threadA.join()
    print('Python Done')
    
    #while True:
    #    print("I am done")
    #    sleep(1)
    
cdef runSelfPlayC(int numSimulations, np.ndarray[DTYPE_INT_t] gameState, np.ndarray[DTYPE_t] v, np.ndarray[DTYPE_t] p, np.ndarray[DTYPE_INT_t] isCReady, np.ndarray[DTYPE_INT_t] isModelReady, np.ndarray[DTYPE_INT_t] error):
    selfPlayCython(numSimulations, <int *> gameState.data, <double *> v.data, <double *> p.data, <int *> isCReady.data, <int *> isModelReady.data, <int *> error.data)

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
