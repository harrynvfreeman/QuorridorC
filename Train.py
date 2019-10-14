from keras.models import load_model
from loss import softmax_cross_entropy_with_logits
from datetime import datetime
from glob import glob
from shutil import move
from os import mkdir, rmdir
from os.path import basename
from SavedState import SavedState, readSavedState
import numpy as np
import schedule

def train(modelName):
    timeStamp, numFiles = moveFilesForProcessing(modelName)
    
    if numFiles == 0:
        rmdir(timeStamp)
        return
    
    gameStates = np.zeros((numFiles, 9, 9, 65))
    pis = np.zeros((numFiles, 140))
    values = np.zeros((numFiles, 1))
    count = 0
    for file in glob('./' + timeStamp + '/' + modelName + '*'):
        savedState = readSavedState(file)
        gameStates[count] = savedState.gameState
        pis[count] = savedState.pi
        values[count] = savedState.value
    
    y = {'valueHead' : values, 'policyHead': pis}
    model = load_model('./models/' + modelName + '.h5', custom_objects={'softmax_cross_entropy_with_logits': softmax_cross_entropy_with_logits})
    model.save('./depModels/' + modelName + '-' + datetime.now().strftime("%d-%b-%Y-%H-%M-%S-%f") + '.h5')
    
    model.fit(gameStates, y, batch_size=32, epochs=1)
    model.save('./models/' + modelName + '.h5')
    
    moveFilesDoneProcessing(timeStamp)
    
def moveFilesForProcessing(modelName):
    timeStamp = datetime.now().strftime("%d-%b-%Y-%H-%M-%S-%f") + '-' + modelName
    mkdir(timeStamp)
    numFiles = 0
    for f in glob('./positions/' + modelName + '*'):
        tempFolder = datetime.now().strftime("%d-%b-%Y-%H-%M-%S-%f")
        move(f, timeStamp + '/' + basename(f))
        numFiles = numFiles + 1
        
    return timeStamp, numFiles

def moveFilesDoneProcessing(timeStamp):
    mkdir('processed/' + timeStamp)
    for f in glob('./' + timeStamp + '/*'):
        move(f, 'processed/' + timeStamp + '/' + basename(f))
    rmdir(timeStamp)
    
def constTrain():
    schedule.every().hour.do(train, args=('model1'))