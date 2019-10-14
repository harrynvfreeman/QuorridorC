from math import sqrt
from QuorridorEnvironment import QuorridorEnvironment as QE
import numpy as np
import weakref
from threading import Lock

Cpuct = 1
e = 0.25
dirichlet = 0.03
TEMP_TURN = 100

class Node():
    def __init__(self, state):
        self.state = state
        self.actions = None
        self.parentAction = None
        self.lock = Lock()
    
    def expandAndEvaluate(self, v, p):
        p = np.exp(p)
        pSum = np.sum(p)
        validMoves = self.state.getValidMoves()
        self.actions = []
        
        [self.doValidMove(validMove, p, pSum) for validMove in validMoves]
        self.backup(v)
        
    def doValidMove(self, validMove, p, pSum):
        newState = QE()
        newState.copy(self.state)
        newState.step(validMove)
        childNode = Node(newState)
        action = Edge(p[validMove]/pSum, childNode, self, validMove)
        childNode.parentAction = weakref.ref(action)
        self.actions.append(action)
        
    def getActions(self):
        return self.actions
     
    def backup(self, v):
        node = self
        while(node.parentAction):
            action = node.parentAction()
            action.lock.acquire()
            action.vLoss = action.vLoss - 1
            #if shouldUpdate:
            action.W = action.W + v
            #else:
                #action.N = action.N - 1
            action.Q = (action.W - action.vLoss) / action.N
            action.lock.release()
            node = action.parent()
            
    def select(self, returnNodes, returnNodeIndex):
        node = self
        while(node.actions):
            node.lock.acquire()
            Q = node.actions[0].Q
            if (node.parentAction):
                N = node.parentAction().N
                P = node.actions[0].P
            else:
                d = np.random.dirichlet([0.03]*len(node.actions))
                N = np.sum(action.N for action in node.actions)
                P = (1-e)*node.actions[0].P + e*d[0]
            U = Cpuct * P * sqrt(N) / (node.actions[0].N + 1)
            maxVal = Q + U
            maxAction = node.actions[0]
            
            index = 1
            for action in node.actions[1:]:
                Q = action.Q
                if (node.parentAction):
                    P = action.P
                else:
                    P = (1-e)*action.P + e*d[index]
                    index = index + 1
                U = Cpuct * action.P * sqrt(N) / (action.N + 1)
                if (Q + U > maxVal):
                    maxVal = Q + U
                    maxAction = action
            maxAction.lock.acquire()
            maxAction.vLoss = maxAction.vLoss + 1
            maxAction.N = maxAction.N + 1
            maxAction.Q = (maxAction.W - maxAction.vLoss) / maxAction.N
            maxAction.lock.release()
            node.lock.release()
            node = maxAction.child
        returnNodes[returnNodeIndex] = node
        
class Edge():
    def __init__(self, P, child, parent, move):
        self.N = 0
        self.W = 0
        self.Q = 0
        self.P = P
        self.vLoss = 0
        self.child = child
        self.parent = weakref.ref(parent)
        self.move = move
        self.lock = Lock()
        
class Tree():
    def __init__(self, rootNode):
        self.rootNode = rootNode
        self.rootNode.parentAction = None
        
    def play(self):
        pi = np.zeros((140))
        if self.rootNode.state.turnNum < TEMP_TURN:
            N = sum(action.N for action in self.rootNode.actions)
            actionMap = dict()
            for action in self.rootNode.actions:
                pi[action.move] = action.N / N
                actionMap[action.move] = action
            move = np.random.choice(len(pi), p=pi)
            nextAction = actionMap[move]
        else:
            moveIndex = np.argmax([action.N for action in self.rootNode.actions])
            nextAction = self.rootNode.actions[moveIndex]
            pi[nextAction.move] = 1
        
        self.rootNode = nextAction.child
        self.rootNode.parentAction = None
        return pi
    
    def selectMove(self):
        moveIndex = np.argmax([action.N for action in self.rootNode.actions])
        nextAction = self.rootNode.actions[moveIndex]
        move = nextAction.move
                
        return move
        
    
    
        
        