import threading
from QuorridorEnvironment import QuorridorEnvironment as QE
from MCTS import Tree, Node
from SavedState import SavedState, writeSavedState
import numpy as np
#from keras.models import load_model
#from loss import softmax_cross_entropy_with_logits
from datetime import datetime
#import sys

#current model path model1.h5
def constRun():
    while True:
        run(8, 400, 'model1')

def run(numThreads, numSimulations, modelName):
    #model = load_model('./models/' + modelName + '.h5', custom_objects={'softmax_cross_entropy_with_logits': softmax_cross_entropy_with_logits})
    
    env = QE()
    env.reset() 
    tree = Tree(Node(env))
    
    gameStates = []
    players = []
    pis = []
    envs = []
    
    testBool = True
    #while tree.rootNode.state.winner == None:
    while testBool:
        testBool = False
        gameStates.append(tree.rootNode.state.gameState)
        players.append(1*(tree.rootNode.state.playerA==tree.rootNode.state.currPlayer) + -1*(tree.rootNode.state.playerB==tree.rootNode.state.currPlayer))
        envs.append(tree.rootNode.state)
    
        pi = tree.play()
        pis.append(pi)
    
    winner = tree.rootNode.state.winner
    for i in range(len(players)):
        gameState = gameStates[i]
        pi = pis[i]
        player = players[i]
        env = envs[i]
        if winner == 0:
            value = 0
        elif winner == player:
            value = 1
        elif winner == -player:
            value = -1
        else:
            raise Exception("Unrecognized Winner")
        savedState = SavedState(gameState, pi, value, env)
        savedPath = "./positions/" + modelName + "-" + datetime.now().strftime("%d-%b-%Y-%H-%M-%S-%f")
        writeSavedState(savedState, savedPath)
    
    return winner
    #return tree

def search(numThreads, numSimulations, rootNode):
    simulationsRun = 0
    #Should I add safety?
    while simulationsRun < numSimulations:
        modelInput = np.zeros((numThreads, 9, 9, 65))
        threads = [None]*numThreads
        returnNodes = [None]*numThreads
        for i in range(numThreads):
            threads[i] = threading.Thread(target=rootNode.select, args=(returnNodes, i))
            threads[i].start()
        repeatNodes = set()
        nodesToExpand = set()
        for i in range(numThreads):
            threads[i].join()
            if not(returnNodes[i] in repeatNodes):
                modelInput[i] = returnNodes[i].state.gameState
                repeatNodes.add(returnNodes[i])
                nodesToExpand.add(i)
        #v, p = model.predict(modelInput)
        v = np.random.random((numThreads, 1))
        p = np.random.random((numThreads, 140))
        for i in nodesToExpand:
            threads[i] = threading.Thread(target=returnNodes[i].expandAndEvaluate, args=(v[i][0], p[i]))
            threads[i].start()
            simulationsRun = simulationsRun + 1
        
        for i in nodesToExpand:
            threads[i].join()
class TestClass():    
    def testRun(self):
        print(datetime.now().strftime("%d-%b-%Y-%H-%M-%S-%f"))
    
        
def runTest():
    t = TestClass()
    x = threading.Thread(target=t.testRun)
    x.start()
    x = threading.Thread(target=t.testRun)
    x.start()
    x = threading.Thread(target=t.testRun)
    x.start()
    print('Done')