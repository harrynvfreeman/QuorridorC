from QuorridorEnvironment import QuorridorEnvironment as QE
from Agent import Agent
#from model import buildModel
import numpy as np
from model import inputShape as inputShape
from QuorridorEnvironment import BOARD_HEIGHT, BOARD_WIDTH
import random
from Utils import valToPosVertBlock, valToPosHorizBlock
from MCTS import Tree, Node
class Game():
    def __init__(self):
        self.env = None
        self.agent = None
        self.prevEnv = None
        self.gameState = None
        
    def createModel(self, name, path):
        model = buildModel()
        model.save(path + '/' + name + '.h5')

    def playGame(self):
        self.env = QE()
        self.env.reset()
        self.agent = Agent()
        #self.agent.loadModel('model1', '/Users/HarryFreeman/Documents/Quorridor/Quorridor/models')
        self.gameState = np.zeros((inputShape), dtype=np.float32)
    
        safetyCount = 0
    
        actions = []
        renders = []
    
        while self.env.winner == None and safetyCount < 200:
            self.updateGameState()
            action = self.agent.pickAction(self.gameState, self.env)
            actions.append(action)
            h = self.env.calcStateHash()
            self.env.step(action)
            self.env.unStep(action)
            if not(h==self.env.calcStateHash()):
                print('Bad: ' + str(action))
                print(h)
                print(self.env.calcStateHash())
            self.env.step(action)
            renders.append(self.env.render(mode='no_draw'))
            safetyCount = safetyCount + 1
    
        if (safetyCount < 200):
            if self.env.winner == 0:
                print('Draw')
            elif self.env.winner == 1:
                print("Winner is: " + self.env.playerA.name)
            else:
                print("Winner is: " + self.env.playerB.name)
        else:
            print("Came didn't end")
        
        return actions, renders
    
def selfPlay():
    env = QE()
    env.reset() 
    tree = Tree(Node(env))
    while(tree.rootNode.state.winner == None):
        print('Here')
        for i in range(10):
            tree.search()
        
        pi = tree.play()
    print(tree.rootNode.state.winner)
    
    # def updateGameState(self):
    #     self.gameState[:, :, 0:6] = self.gameState[:, :, 1:7]
    #     self.gameState[:, :, 8:14] = self.gameState[:, :, 9:15]
    #     self.gameState[:, :, 16:22] = self.gameState[:, :, 17:23]
    #     self.gameState[:, :, 24:30] = self.gameState[:, :, 25:31]
    #     self.gameState[:, :, 32:38] = self.gameState[:, :, 33:39]
    #     self.gameState[:, :, 40:46] = self.gameState[:, :, 41:47]
    #     self.gameState[:, :, 48:54] = self.gameState[:, :, 49:55]
    #     self.gameState[:, :, 56:62] = self.gameState[:, :, 57:63]
    #     #player A
    #     self.gameState[:, :, 7] = 0
    #     self.gameState[self.env.playerA.yPos, self.env.playerA.xPos, 7] = 1
    #     #player B
    #     self.gameState[:, :, 15] = 0
    #     self.gameState[self.env.playerB.yPos, self.env.playerB.xPos, 15] = 1
    #     #playerA blocks in hand
    #     self.gameState[:, :, 23] = 0
    #     randomPlaceA = np.array(random.sample(range(0, BOARD_HEIGHT*BOARD_WIDTH), self.env.playerA.numBlocks))
    #     if randomPlaceA.shape[0] > 0:
    #         self.gameState[randomPlaceA // BOARD_WIDTH, randomPlaceA % BOARD_WIDTH, 23] = 1
    #     #playerB blocks on
    #     self.gameState[:, :, 31] = 0
    #     randomPlaceB = np.array(random.sample(range(0, BOARD_HEIGHT*BOARD_WIDTH), self.env.playerB.numBlocks))
    #     if randomPlaceB.shape[0] > 0:
    #         self.gameState[randomPlaceB // BOARD_WIDTH, randomPlaceB % BOARD_WIDTH, 31] = 1
    #     #Blocks on board vert
    #     self.gameState[0:BOARD_HEIGHT-1, 0:BOARD_WIDTH-1, 39] = 1-self.env.availBlockVertPlace.reshape((BOARD_HEIGHT-1, BOARD_WIDTH-1))
    #     #Blocks on board horiz
    #     self.gameState[0:BOARD_WIDTH-1, 0:BOARD_HEIGHT-1, 47] = 1-self.env.availBlockHorizPlace.reshape((BOARD_WIDTH-1, BOARD_HEIGHT-1))
    #     #has been repeated
    #     self.gameState[:, :, 55] = self.env.pastStates[self.env.calcStateHash()] == 2
    #     self.gameState[:, :, 63] = self.env.pastStates[self.env.calcStateHash()] == 3
    #     #whose turn
    #     if self.env.currPlayer == self.env.playerA:
    #         self.gameState[:,:,64] = 1
    #     else:
    #         self.gameState[:,:,64] = 0
    
    