#from keras.models import load_model
from loss import softmax_cross_entropy_with_logits
from datetime import datetime
from glob import glob
from shutil import move
from os import mkdir, rmdir
from os.path import basename
from SavedState import SavedState, readSavedState, VersionDescriptor, writeSavedState
import numpy as np

def train(model):
    
    versionDescriptor = readSavedState("./models/version")
    version = versionDescriptor.version
    
    timeStamp, numFiles = moveFilesForProcessing(version)
    
    if numFiles == 0:
        rmdir(timeStamp)
        return
    #timeStamp = 'positionsBeingProcessed-0-25-Oct-2019-04-30-51-754982'
    #numFiles = 6000
    gameStates = np.zeros((numFiles, 17, 17, 29))
    pis = np.zeros((numFiles, 140))
    values = np.zeros((numFiles, 1))
    count = 0
    for file in glob('./' + timeStamp + '/position*'):
        savedState = readSavedState(file)
        gameStates[count] = savedState.gameState
        pis[count] = savedState.pi
        values[count] = savedState.value
    
    y = {'valueHead' : values, 'policyHead': pis}
    #model = load_model('./models/model.h5', custom_objects={'softmax_cross_entropy_with_logits': softmax_cross_entropy_with_logits})
    #NOT Doing model.save('./depModels/' + modelName + '-' + datetime.now().strftime("%d-%b-%Y-%H-%M-%S-%f") + '.h5')
    
    model.fit(gameStates, y, batch_size=32, epochs=50)
    
    model.save('./models/model.h5')
    newVersion = version + 1
    newVersionDescriptor = VersionDescriptor(newVersion)
    writeSavedState(newVersionDescriptor, './models/version')
    
    moveFilesDoneProcessing(timeStamp, version)
    
def moveFilesForProcessing(version):
    timeStamp = './positionsBeingProcessed-' + str(version) + "-" + datetime.now().strftime("%d-%b-%Y-%H-%M-%S-%f")
    mkdir(timeStamp)
    numFiles = 0
    for f in glob('./positionsToBeProcessed/position*'):
        move(f, "./" + timeStamp + '/' + basename(f))
        numFiles = numFiles + 1
        
    return timeStamp, numFiles

def moveFilesDoneProcessing(timeStamp, version):
    mkdir('./positionsProcessed/' + str(version))
    for f in glob('./' + timeStamp + '/position*'):
        move(f, './positionsProcessed/' + str(version) + "/" + basename(f))
    rmdir(timeStamp)
    
def constTrain():
    schedule.every().hour.do(train, args=('model1'))