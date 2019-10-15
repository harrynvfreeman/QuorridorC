import numpy as np
cimport numpy as np
#from keras.models import load_model
#loss import softmax_cross_entropy_with_logits
from time import sleep
from numpy cimport ndarray
import threading
from libc.stdlib cimport malloc, free

DTYPE = np.float32
ctypedef np.float32_t DTYPE_t

DTYPE_INT = np.int32
ctypedef np.int32_t DTYPE_INT_t

#model = load_model('./models/' + modelName + '.h5', custom_objects={'softmax_cross_entropy_with_logits': softmax_cross_entropy_with_logits})

cdef extern from "Quorridor.h":
    void selfPlayCython(int numSimulations, int * gameState, double * v, double * p, int * isCReady, int * isModelReady, int * error);
    int NUM_MOVES
    int NUM_CHANNELS
    int NUM_ROWS
    int NUM_COLS
    
    
cpdef selfPlay():
    cdef np.ndarray[DTYPE_INT_t] gameState
    cdef int * gameStatePointer
    cdef np.ndarray[DTYPE_t] v
    cdef double * vPointer
    cdef np.ndarray[DTYPE_t] p
    cdef double * pPointer
    cdef np.ndarray[DTYPE_INT_t] isCReady
    cdef int * isCReadyPointer
    cdef np.ndarray[DTYPE_INT_t] isModelReady
    cdef int * isModelReadyPointer
    cdef np.ndarray[DTYPE_INT_t] error
    cdef int * errorPointer
    
    gameState = np.zeros((NUM_CHANNELS*NUM_ROWS*NUM_COLS), dtype=DTYPE_INT)
    gameStatePointer = <int *> gameState.data
    v = np.zeros((1), dtype=DTYPE)
    vPointer = <double *> v.data
    p = np.zeros((NUM_MOVES), dtype=DTYPE)
    pPointer = <double *> p.data
    isCReady = np.zeros((1), dtype=DTYPE_INT)
    isCReadyPointer = <int *> isCReady.data
    isModelReady = np.zeros((1), dtype=DTYPE_INT)
    isModelReadyPointer = <int *> isModelReady.data
    error = np.zeros((1), dtype=DTYPE_INT)
    errorPointer = <int *> error.data
    
    selfPlayThread = threading.Thread(target=callSelfPlayCython, args=(gameState, v, p, isCReady,isModelReady, error,))
    #testThread = threading.Thread(target=threadTest, args=(1,))
    print('PreStart')
    #testThread.start()
    selfPlayThread.start()
    print('Started')
    #start with random
    while selfPlayThread.is_alive():
        #print('here python0')
        if (isCReadyPointer[0] == 1):
            isCReadyPointer[0] = 0
            #print('here python1')
            vTemp = np.random.random((1, 1))
            pTemp = np.random.random((1, 140))
            
            vPointer[0] = vTemp[0][0]
            
            for i in range(NUM_MOVES):
                print(pTemp[0][i])
                print(i)
                pPointer[i] = pTemp[0][i]
            print('setting: ' + str(isModelReadyPointer[0]))
            isModelReadyPointer[0] = 1
            print('set: ' + str(isModelReadyPointer[0]))
        
        sleep(0.5)
    selfPlayThread.join()
    #free(gameStatePointer)
    #free(vPointer)
    #free(pPointer)
    #free(isCReadyPointer)
    #free(isModelReadyPointer)
    #free(errorPointer)
    print('Dunzo')
    
cdef callSelfPlayCython(np.ndarray[DTYPE_INT_t] gameStatePointer, np.ndarray[DTYPE_t] vPointer, np.ndarray[DTYPE_t] pPointer, np.ndarray[DTYPE_INT_t] isCReadyPointer, np.ndarray[DTYPE_INT_t] isModelReadyPointer, np.ndarray[DTYPE_INT_t] errorPointer):
    print('Holaaaa')
    selfPlayCython(400, <int *>gameStatePointer.data, <double *>vPointer.data, <double *>pPointer.data, <int *>isCReadyPointer.data, <int *>isModelReadyPointer.data, <int *>errorPointer.data)     
    print('Dunzaaaaa')
    return
    
cdef threadTest(int a):
    while True:
        print('testworks')
        sleep(1)
