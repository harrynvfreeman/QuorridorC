import gym
from Player import Player
from Tile import Tile
from Utils import valToPosVertBlock, posToValVertBlock, valToPosHorizBlock, posToValHorizBlock, displayRender
import random
import numpy as np
import sys

BOARD_WIDTH = 9
BOARD_HEIGHT = 9
NUM_BLOCKS = 10

inputShape = (BOARD_HEIGHT,BOARD_WIDTH,65)

CONST_NUM = 12

CONST_MAX_MOVE = 40

SPACE_SIZE = CONST_NUM - 1 + 2*(BOARD_HEIGHT-1)*(BOARD_WIDTH-1)

class QuorridorEnvironment(gym.Env):
    def __init__(self):
        self.playerA = None
        self.playerB = None
        self.board = None
        self.currPlayer = None
        self.winner = None
        self.availBlockVertPlace = None
        self.availBlockHorizPlace = None
        self.turnNum = None
        self.pastStates = None
        self.gameState = None
        
    def reset(self):
        self.playerA = Player(yPos=0, yTarget=BOARD_HEIGHT-1, xPos=BOARD_WIDTH//2, numBlocks=NUM_BLOCKS, name="PlayerA")
        self.playerB = Player(yPos=BOARD_HEIGHT-1, yTarget=0, xPos=BOARD_WIDTH//2, numBlocks=NUM_BLOCKS, name="PlayerB")
        self.playerA.nextPlayer = self.playerB
        self.playerB.nextPlayer = self.playerA
        self.board = initializeBoard(BOARD_HEIGHT, BOARD_WIDTH)
        self.availBlockVertPlace = np.ones((BOARD_HEIGHT - 1)*(BOARD_WIDTH - 1))
        self.availBlockHorizPlace = np.ones((BOARD_HEIGHT - 1)*(BOARD_WIDTH - 1))
        self.currPlayer = self.playerA
        self.winner = None
        self.turnNum = 0
        self.pastStates = dict()
        self.pastStates[self.calcStateHash()] = 1
        self.gameState = np.zeros((inputShape))
        self.updateGameState()
        
    def copy(self, cloneFrom):
        self.playerA = Player(yPos=cloneFrom.playerA.yPos, yTarget=cloneFrom.playerA.yTarget, xPos=cloneFrom.playerA.xPos, numBlocks=cloneFrom.playerA.numBlocks, name=cloneFrom.playerA.name)
        self.playerB = Player(yPos=cloneFrom.playerB.yPos, yTarget=cloneFrom.playerB.yTarget, xPos=cloneFrom.playerB.xPos, numBlocks=cloneFrom.playerB.numBlocks, name=cloneFrom.playerB.name)
        self.playerA.nextPlayer = self.playerB
        self.playerB.nextPlayer = self.playerA
        self.availBlockVertPlace = np.copy(cloneFrom.availBlockVertPlace)
        self.availBlockHorizPlace = np.copy(cloneFrom.availBlockHorizPlace)
        self.board = copyBoard(cloneFrom.board)
        self.winner = cloneFrom.winner
        self.turnNum = cloneFrom.turnNum
        self.pastStates = cloneFrom.pastStates.copy()
        self.gameState = np.copy(cloneFrom.gameState)
        
        if cloneFrom.currPlayer == cloneFrom.playerA:
            self.currPlayer = self.playerA
        elif cloneFrom.currPlayer == cloneFrom.playerB:
            self.currPlayer = self.playerB
        else:
            raise Exception("Invalid curr player")
      
    ###WARNING WARNING WARNING
    ###hardcoded now for simplicity
    def render(self, mode='human'):
        draw = []
        for row in reversed(range(17)):
            if row%2 == 0:
                draw.append(self.renderTileRow(row))
            else:
                draw.append(self.renderBlockRow(row))
                
        # for row in range(1, 17, 2):
        #     for col in range(1, 17, 2):
        #         if draw[row-1][col] == "|" and draw[row+1][col] == "|":
        #             draw[row][col] = "|"
        #         elif draw[row][col-1] == "--" and draw[row][col+1] == "--":
        #             draw[row][col] = "--"
        if mode == 'human': 
            displayRender(draw)
            
        return draw
        
    def renderTileRow(self, row):
        draw = []
        for col in range(17):
            if col%2 == 0:
                if self.playerA.yPos == row//2 and self.playerA.xPos == col//2:
                    draw.append("A")
                elif self.playerB.yPos == row//2 and self.playerB.xPos == col//2:
                    draw.append("B")
                else:
                    draw.append("*")
            else:
                if self.board[row//2, col//2 + 1] in self.board[row//2, col//2].neighbours:
                    draw.append("")
                else:
                    draw.append("|")
        return draw
    
    def renderBlockRow(self, row):
        draw = []
        for col in range(17):
            if col%2 == 0:
                if self.board[row//2 + 1, col//2] in self.board[row//2, col//2].neighbours:
                    draw.append("")
                else:
                    draw.append("--")
            else:
                draw.append("")
        return draw
        
    def validate(self, action):
        if not(self.winner == None):
            raise Exception("Already Winner")
        
        if action == 0:
            return self.isMovePlayerUpValid()
        elif action == 1:
            return self.isMovePlayerRightValid()
        elif action == 2:
            return self.isMovePlayerDownValid()
        elif action == 3:
            return self.isMovePlayerLeftValid()
        elif action == 4:
            return self.isMovePlayerTwoUpValid()
        elif action == 5:
            return self.isMovePlayerTwoRightValid()
        elif action == 6:
            return self.isMovePlayerTwoDownValid()
        elif action == 7:
            return self.isMovePlayerTwoLeftValid()
        elif action == 8:
            return self.isMovePlayerUpRightValid()
        elif action == 9:
            return self.isMovePlayerDownRightValid()
        elif action == 10:
            return self.isMovePlayerDownLeftValid()
        elif action == 11:
            return self.isMovePlayerUpLeftValid()
        elif action >= CONST_NUM and action < CONST_NUM + (BOARD_HEIGHT-1)*(BOARD_WIDTH-1):
            return self.isPlaceVerticalBlockValid(action - CONST_NUM)
        elif action >= CONST_NUM + (BOARD_HEIGHT-1)*(BOARD_WIDTH-1) and action < CONST_NUM + 2*(BOARD_HEIGHT-1)*(BOARD_WIDTH-1):
            return self.isPlaceHorizontalBlockValid(action - CONST_NUM - (BOARD_HEIGHT-1)*(BOARD_WIDTH-1))
        raise Exception("Invalid Move")
    
    #assuming valid action
    def step(self, action):
        
        if action == 0:
            self.movePlayerUp()
        elif action == 1:
            self.movePlayerRight()
        elif action == 2:
            self.movePlayerDown()
        elif action == 3:
            self.movePlayerLeft()
        elif action == 4:
            self.movePlayerTwoUp()
        elif action == 5:
            self.movePlayerTwoRight()
        elif action == 6:
            self.movePlayerTwoDown()
        elif action == 7:
            self.movePlayerTwoLeft()
        elif action == 8:
            self.movePlayerUpRight()
        elif action == 9:
            self.movePlayerDownRight()
        elif action == 10:
            self.movePlayerDownLeft()
        elif action == 11:
            self.movePlayerUpLeft()
        elif action >= CONST_NUM and action < CONST_NUM + (BOARD_HEIGHT-1)*(BOARD_WIDTH-1):
            self.placeVerticalBlock(action - CONST_NUM)
        elif action >= CONST_NUM + (BOARD_HEIGHT-1)*(BOARD_WIDTH-1) and action < CONST_NUM + 2*(BOARD_HEIGHT-1)*(BOARD_WIDTH-1):
            self.placeHorizontalBlock(action - CONST_NUM - (BOARD_HEIGHT-1)*(BOARD_WIDTH-1))
        else:
            raise Exception("Illegal movoe")
        
        self.turnNum = self.turnNum + 1
        self.currPlayer = self.currPlayer.nextPlayer
        
        h = self.calcStateHash()
        if h in self.pastStates:
            if self.pastStates[h] == 2:
                self.winner = 0 if self.winner == None else self.winner
                self.pastStates[h] = 3
            else:
                self.pastStates[h] = 2
        else:
            self.pastStates[h] = 1
            
        self.updateGameState()
        
    def movePlayerUp(self):
        self.currPlayer.yPos = self.currPlayer.yPos+1
        if (self.currPlayer.yPos == self.currPlayer.yTarget):
            self.winner = 1*(self.currPlayer == self.playerA) + -1*(self.currPlayer == self.playerB)
    
    def isMovePlayerUpValid(self):
        if (self.currPlayer.yPos >= BOARD_HEIGHT-1):
            return False
        
        currTile = self.board[self.currPlayer.yPos, self.currPlayer.xPos]
        nextTile = self.board[self.currPlayer.yPos + 1, self.currPlayer.xPos]
        if (not(nextTile in currTile.neighbours)):
            return False
        
        if (self.currPlayer.yPos+1== self.currPlayer.nextPlayer.yPos and self.currPlayer.xPos == self.currPlayer.nextPlayer.xPos):
            return False
        
        return True
             
    def movePlayerDown(self):
        self.currPlayer.yPos = self.currPlayer.yPos-1
        if (self.currPlayer.yPos == self.currPlayer.yTarget):
            self.winner = 1*(self.currPlayer == self.playerA) + -1*(self.currPlayer == self.playerB)
            
    def isMovePlayerDownValid(self):
        if (self.currPlayer.yPos <= 0 ):
            return False
        
        currTile = self.board[self.currPlayer.yPos, self.currPlayer.xPos]
        nextTile = self.board[self.currPlayer.yPos - 1, self.currPlayer.xPos]
        if (not(nextTile in currTile.neighbours)):
            return False
        
        if (self.currPlayer.yPos-1== self.currPlayer.nextPlayer.yPos and self.currPlayer.xPos == self.currPlayer.nextPlayer.xPos):
            return False
        return True
    
    def movePlayerRight(self):
        self.currPlayer.xPos = self.currPlayer.xPos+1
        
    def isMovePlayerRightValid(self):
        if (self.currPlayer.xPos >= BOARD_WIDTH-1):
            return False
        
        currTile = self.board[self.currPlayer.yPos, self.currPlayer.xPos]
        nextTile = self.board[self.currPlayer.yPos, self.currPlayer.xPos + 1]
        if (not(nextTile in currTile.neighbours)):
            return False
        
        if (self.currPlayer.xPos+1== self.currPlayer.nextPlayer.xPos and self.currPlayer.yPos == self.currPlayer.nextPlayer.yPos):
            return False
        
        return True
       
    def movePlayerLeft(self):
        self.currPlayer.xPos = self.currPlayer.xPos-1
        
    def isMovePlayerLeftValid(self):
        if (self.currPlayer.xPos <= 0):
            return False
        
        currTile = self.board[self.currPlayer.yPos, self.currPlayer.xPos]
        nextTile = self.board[self.currPlayer.yPos, self.currPlayer.xPos - 1]
        if (not(nextTile in currTile.neighbours)):
            return False
        
        if (self.currPlayer.xPos-1== self.currPlayer.nextPlayer.xPos and self.currPlayer.yPos == self.currPlayer.nextPlayer.yPos):
            return False
        
        return True
    
    def movePlayerTwoUp(self):
        self.currPlayer.yPos = self.currPlayer.yPos+2
        if (self.currPlayer.yPos == self.currPlayer.yTarget):
            self.winner = 1*(self.currPlayer == self.playerA) + -1*(self.currPlayer == self.playerB)

    def isMovePlayerTwoUpValid(self):
        if (self.currPlayer.yPos >= BOARD_HEIGHT-2):
            return False
        
        currTile = self.board[self.currPlayer.yPos, self.currPlayer.xPos]
        midTile = self.board[self.currPlayer.yPos + 1, self.currPlayer.xPos]
        nextTile = self.board[self.currPlayer.yPos + 2, self.currPlayer.xPos]
        if (not(midTile in currTile.neighbours) or not(nextTile in midTile.neighbours)):
            return False
        
        if (not(self.currPlayer.yPos+1== self.currPlayer.nextPlayer.yPos and self.currPlayer.xPos == self.currPlayer.nextPlayer.xPos)):
            return False
        
        return True
            
    def movePlayerTwoDown(self):
        self.currPlayer.yPos = self.currPlayer.yPos-2
        if (self.currPlayer.yPos == self.currPlayer.yTarget):
            self.winner = 1*(self.currPlayer == self.playerA) + -1*(self.currPlayer == self.playerB)

    def isMovePlayerTwoDownValid(self):
        if (self.currPlayer.yPos <= 1):
            return False
        
        currTile = self.board[self.currPlayer.yPos, self.currPlayer.xPos]
        midTile = self.board[self.currPlayer.yPos - 1, self.currPlayer.xPos]
        nextTile = self.board[self.currPlayer.yPos - 2, self.currPlayer.xPos]
        if (not(midTile in currTile.neighbours) or not(nextTile in midTile.neighbours)):
            return False
        
        if (not(self.currPlayer.yPos-1== self.currPlayer.nextPlayer.yPos and self.currPlayer.xPos == self.currPlayer.nextPlayer.xPos)):
            return False
        
        return True
        
    def movePlayerTwoRight(self):
        self.currPlayer.xPos = self.currPlayer.xPos+2
        
    def isMovePlayerTwoRightValid(self):
        if (self.currPlayer.xPos >= BOARD_WIDTH-2):
            return False
        
        currTile = self.board[self.currPlayer.yPos, self.currPlayer.xPos]
        midTile = self.board[self.currPlayer.yPos, self.currPlayer.xPos + 1]
        nextTile = self.board[self.currPlayer.yPos, self.currPlayer.xPos + 2]
        if (not(midTile in currTile.neighbours) or not(nextTile in midTile.neighbours)):
            return False
        
        if (not(self.currPlayer.xPos+1== self.currPlayer.nextPlayer.xPos and self.currPlayer.yPos == self.currPlayer.nextPlayer.yPos)):
            return False
        
        return True

    def movePlayerTwoLeft(self):
        self.currPlayer.xPos = self.currPlayer.xPos-2
        
    def isMovePlayerTwoLeftValid(self):
        if (self.currPlayer.xPos <= 1):
            return False
        
        currTile = self.board[self.currPlayer.yPos, self.currPlayer.xPos]
        midTile = self.board[self.currPlayer.yPos, self.currPlayer.xPos - 1]
        nextTile = self.board[self.currPlayer.yPos, self.currPlayer.xPos - 2]
        if (not(midTile in currTile.neighbours) or not(nextTile in midTile.neighbours)):
            return False
        
        if (not(self.currPlayer.xPos-1== self.currPlayer.nextPlayer.xPos and self.currPlayer.yPos == self.currPlayer.nextPlayer.yPos)):
            return False
        
        return True
    
    def movePlayerUpRight(self):
        self.currPlayer.yPos = self.currPlayer.yPos+1
        self.currPlayer.xPos = self.currPlayer.xPos+1
        if (self.currPlayer.yPos == self.currPlayer.yTarget):
            self.winner = 1*(self.currPlayer == self.playerA) + -1*(self.currPlayer == self.playerB)
            
    def isMovePlayerUpRightValid(self):
        if (self.currPlayer.yPos >= BOARD_HEIGHT-1 or self.currPlayer.xPos >= BOARD_WIDTH-1):
            return False
        
        currTile = self.board[self.currPlayer.yPos, self.currPlayer.xPos]
        midTile0 = self.board[self.currPlayer.yPos + 1, self.currPlayer.xPos]
        midTile1 = self.board[self.currPlayer.yPos, self.currPlayer.xPos + 1]
        nextTile = self.board[self.currPlayer.yPos + 1, self.currPlayer.xPos + 1]
        if (not(midTile0 in currTile.neighbours and nextTile in midTile0.neighbours and self.currPlayer.yPos + 1 == self.currPlayer.nextPlayer.yPos \
        and self.currPlayer.xPos == self.currPlayer.nextPlayer.xPos) and \
        not(midTile1 in currTile.neighbours and nextTile in midTile1.neighbours and self.currPlayer.xPos + 1 == self.currPlayer.nextPlayer.xPos \
        and self.currPlayer.yPos == self.currPlayer.nextPlayer.yPos)):
            return False
        
        return True
            
    def movePlayerDownRight(self):
        self.currPlayer.yPos = self.currPlayer.yPos-1
        self.currPlayer.xPos = self.currPlayer.xPos+1
        if (self.currPlayer.yPos == self.currPlayer.yTarget):
            self.winner = 1*(self.currPlayer == self.playerA) + -1*(self.currPlayer == self.playerB)
            
    def isMovePlayerDownRightValid(self):
        if (self.currPlayer.yPos <= 0 or self.currPlayer.xPos >= BOARD_WIDTH-1):
            return False
        
        currTile = self.board[self.currPlayer.yPos, self.currPlayer.xPos]
        midTile0 = self.board[self.currPlayer.yPos - 1, self.currPlayer.xPos]
        midTile1 = self.board[self.currPlayer.yPos, self.currPlayer.xPos + 1]
        nextTile = self.board[self.currPlayer.yPos - 1, self.currPlayer.xPos + 1]
        if (not(midTile0 in currTile.neighbours and nextTile in midTile0.neighbours and self.currPlayer.yPos - 1 == self.currPlayer.nextPlayer.yPos \
        and self.currPlayer.xPos == self.currPlayer.nextPlayer.xPos) and \
        not(midTile1 in currTile.neighbours and nextTile in midTile1.neighbours and self.currPlayer.xPos + 1 == self.currPlayer.nextPlayer.xPos \
        and self.currPlayer.yPos == self.currPlayer.nextPlayer.yPos)):
            return False
        
        return True
    
    def movePlayerDownLeft(self):
        self.currPlayer.yPos = self.currPlayer.yPos-1
        self.currPlayer.xPos = self.currPlayer.xPos-1
        if (self.currPlayer.yPos == self.currPlayer.yTarget):
            self.winner = 1*(self.currPlayer == self.playerA) + -1*(self.currPlayer == self.playerB)
            
    def isMovePlayerDownLeftValid(self):
        if (self.currPlayer.yPos <= 0 or self.currPlayer.xPos <= 0):
            return False
        
        currTile = self.board[self.currPlayer.yPos, self.currPlayer.xPos]
        midTile0 = self.board[self.currPlayer.yPos - 1, self.currPlayer.xPos]
        midTile1 = self.board[self.currPlayer.yPos, self.currPlayer.xPos - 1]
        nextTile = self.board[self.currPlayer.yPos - 1, self.currPlayer.xPos - 1]
        if (not(midTile0 in currTile.neighbours and nextTile in midTile0.neighbours and self.currPlayer.yPos - 1 == self.currPlayer.nextPlayer.yPos \
        and self.currPlayer.xPos == self.currPlayer.nextPlayer.xPos) and \
        not(midTile1 in currTile.neighbours and nextTile in midTile1.neighbours and self.currPlayer.xPos - 1 == self.currPlayer.nextPlayer.xPos \
        and self.currPlayer.yPos == self.currPlayer.nextPlayer.yPos)):
            return False
        
        return True
    
    def movePlayerUpLeft(self):
        self.currPlayer.yPos = self.currPlayer.yPos+1
        self.currPlayer.xPos = self.currPlayer.xPos-1
        if (self.currPlayer.yPos == self.currPlayer.yTarget):
            self.winner = 1*(self.currPlayer == self.playerA) + -1*(self.currPlayer == self.playerB)
            
    def isMovePlayerUpLeftValid(self):
        if (self.currPlayer.yPos >= BOARD_HEIGHT-1 or self.currPlayer.xPos <= 0):
            return False
        
        currTile = self.board[self.currPlayer.yPos, self.currPlayer.xPos]
        midTile0 = self.board[self.currPlayer.yPos + 1, self.currPlayer.xPos]
        midTile1 = self.board[self.currPlayer.yPos, self.currPlayer.xPos - 1]
        nextTile = self.board[self.currPlayer.yPos + 1, self.currPlayer.xPos - 1]
        if (not(midTile0 in currTile.neighbours and nextTile in midTile0.neighbours and self.currPlayer.yPos + 1 == self.currPlayer.nextPlayer.yPos \
        and self.currPlayer.xPos == self.currPlayer.nextPlayer.xPos) and \
        not(midTile1 in currTile.neighbours and nextTile in midTile1.neighbours and self.currPlayer.xPos - 1 == self.currPlayer.nextPlayer.xPos \
        and self.currPlayer.yPos == self.currPlayer.nextPlayer.yPos)):
            return False
        
        return True
        
    def placeVerticalBlock(self, m):
        ###we need to
        ###place block
        ###remove move from allowed block move
        ###reduce players block num
        ###remove all future illegal block placements
        
        y0, x0, y1, x1, _, _, _, _ = self.putVertBlock(m, "place")
        
        self.availBlockVertPlace[m] = 0
        
        self.currPlayer.numBlocks = self.currPlayer.numBlocks - 1
        
        if (y1 < BOARD_HEIGHT - 1):
            topBlock = posToValVertBlock(y1, x0, BOARD_WIDTH)
            if self.availBlockVertPlace[topBlock]:
                self.availBlockVertPlace[topBlock] = 0
        if (y0 > 0):
            bottomBlock = posToValVertBlock(y0 - 1, x0, BOARD_WIDTH)
            if self.availBlockVertPlace[bottomBlock]:
                self.availBlockVertPlace[bottomBlock] = 0
        
        sliceBlock = posToValHorizBlock(y0, x0, BOARD_HEIGHT)
        if self.availBlockHorizPlace[sliceBlock]:
            self.availBlockHorizPlace[sliceBlock] = 0
            
    def isPlaceVerticalBlockValid(self, m):
        if (self.currPlayer.numBlocks <= 0):
            return False
        
        #covers out of bounds and block in way
        if not(self.availBlockVertPlace[m]):
            return False
        
        ##temporarily place the block to see if player can make it
        _, _, _, _, tile0, tile1, tile2, tile3 = self.putVertBlock(m, "val")
        
        tile = self.board[self.currPlayer.yPos, self.currPlayer.xPos]
        visited = np.zeros((BOARD_HEIGHT, BOARD_WIDTH), dtype=bool)
        if not dfs(tile, self.currPlayer.yTarget, visited):
            pickUpBlock(tile0, tile1, tile2, tile3)
            return False
        
        tile = self.board[self.currPlayer.nextPlayer.yPos, self.currPlayer.nextPlayer.xPos]
        visited = np.zeros((BOARD_HEIGHT, BOARD_WIDTH), dtype=bool)
        if not dfs(tile, self.currPlayer.nextPlayer.yTarget, visited):
            pickUpBlock(tile0, tile1, tile2, tile3)
            return False
        
        #remove placed block
        pickUpBlock(tile0, tile1, tile2, tile3)
        return True
        
    def putVertBlock(self, m, state):
        y0, x0, y1, x1 = valToPosVertBlock(m, BOARD_WIDTH)
        
        tile0 = self.board[y0, x0]
        tile1 = self.board[y0, x1]
        tile0.neighbours.remove(tile1)
        tile1.neighbours.remove(tile0)
        
        tile2 = self.board[y1, x0]
        tile3 = self.board[y1, x1]
        tile2.neighbours.remove(tile3)
        tile3.neighbours.remove(tile2)
        
        return y0, x0, y1, x1, tile0, tile1, tile2, tile3

    def placeHorizontalBlock(self, m):
        ###we need to
        ###place Block
        ###remove move from allowed block move
        ###reduce players block num
        ###remove all future illegal block placements
        
        y0, x0, y1, x1, _, _, _, _ = self.putHorizBlock(m, "play")
        
        self.availBlockHorizPlace[m] = 0
        
        self.currPlayer.numBlocks = self.currPlayer.numBlocks - 1
        
        if (x1 < BOARD_WIDTH - 1):
            rightBlock = posToValHorizBlock(y0, x1, BOARD_HEIGHT)
            if self.availBlockHorizPlace[rightBlock]:
                self.availBlockHorizPlace[rightBlock] = 0
        if (x0 > 0):
            leftBlock = posToValHorizBlock(y0, x0 - 1, BOARD_HEIGHT)
            if self.availBlockHorizPlace[leftBlock]:
                self.availBlockHorizPlace[leftBlock] = 0
            
        sliceBlock = posToValVertBlock(y0, x0, BOARD_WIDTH)
        if self.availBlockVertPlace[sliceBlock]:
            self.availBlockVertPlace[sliceBlock] = 0
            
    def isPlaceHorizontalBlockValid(self, m):
        if (self.currPlayer.numBlocks <= 0):
            return False
        
        #covers out of bounds and block in way
        if not(self.availBlockHorizPlace[m]):
            return False
        
        ##templrarily place the block
        _, _, _, _, tile0, tile1, tile2, tile3 = self.putHorizBlock(m, "val")
        
        ##Here have to check if player can make it
        tile = self.board[self.currPlayer.yPos, self.currPlayer.xPos]
        visited = np.zeros((BOARD_HEIGHT, BOARD_WIDTH), dtype=bool)
        
        if not dfs(tile, self.currPlayer.yTarget, visited):
            pickUpBlock(tile0, tile1, tile2, tile3)
            return False
        
        tile = self.board[self.currPlayer.nextPlayer.yPos, self.currPlayer.nextPlayer.xPos]
        visited = np.zeros((BOARD_HEIGHT, BOARD_WIDTH), dtype=bool)
        if not dfs(tile, self.currPlayer.nextPlayer.yTarget, visited):
            pickUpBlock(tile0, tile1, tile2, tile3)
            return False
        
        pickUpBlock(tile0, tile1, tile2, tile3)
        return True
    
    def putHorizBlock(self, m, state):
        y0, x0, y1, x1 = valToPosHorizBlock(m, BOARD_HEIGHT)
        
        tile0 = self.board[y0, x0]
        tile1 = self.board[y1, x0]
        tile0.neighbours.remove(tile1)
        tile1.neighbours.remove(tile0)
        
        tile2 = self.board[y0, x1]
        tile3 = self.board[y1, x1]
        tile2.neighbours.remove(tile3)
        tile3.neighbours.remove(tile2)
        
        return y0, x0, y1, x1, tile0, tile1, tile2, tile3
            
    def calcStateHash(self):
        h = np.zeros((5+2*(BOARD_HEIGHT-1)*(BOARD_WIDTH-1)), dtype = np.int)
        h[0:5] = [self.currPlayer == self.playerA, self.playerA.yPos, self.playerA.xPos, self.playerB.yPos, self.playerB.xPos]
        h[5:5+(BOARD_HEIGHT-1)*(BOARD_WIDTH-1)] = self.availBlockVertPlace
        h[5+(BOARD_HEIGHT-1)*(BOARD_WIDTH-1):] = self.availBlockHorizPlace
        return tuple(h)
    
    def getValidMoves(self):
        if not(self.winner == None):
            return []
        validMoves = []
        for i in range(12 + 2*(BOARD_HEIGHT-1)*(BOARD_WIDTH-1)):
            if (self.validate(i)):
                validMoves.append(i)
        
        if len(validMoves) == 0:
            raise Exception("Valid moves needs to be greater than 0 here")
        return validMoves
    
    def updateGameState(self):
        self.gameState[:, :, 0:6] = self.gameState[:, :, 1:7]
        self.gameState[:, :, 8:14] = self.gameState[:, :, 9:15]
        self.gameState[:, :, 16:22] = self.gameState[:, :, 17:23]
        self.gameState[:, :, 24:30] = self.gameState[:, :, 25:31]
        self.gameState[:, :, 32:38] = self.gameState[:, :, 33:39]
        self.gameState[:, :, 40:46] = self.gameState[:, :, 41:47]
        self.gameState[:, :, 48:54] = self.gameState[:, :, 49:55]
        self.gameState[:, :, 56:62] = self.gameState[:, :, 57:63]
        #player A
        self.gameState[:, :, 7] = 0
        self.gameState[self.playerA.yPos, self.playerA.xPos, 7] = 1
        #player B
        self.gameState[:, :, 15] = 0
        self.gameState[self.playerB.yPos, self.playerB.xPos, 15] = 1
        #playerA blocks in hand
        self.gameState[:, :, 23] = 0
        randomPlaceA = np.array(random.sample(range(0, BOARD_HEIGHT*BOARD_WIDTH), self.playerA.numBlocks))
        if randomPlaceA.shape[0] > 0:
            self.gameState[randomPlaceA // BOARD_WIDTH, randomPlaceA % BOARD_WIDTH, 23] = 1
        #playerB blocks on
        self.gameState[:, :, 31] = 0
        randomPlaceB = np.array(random.sample(range(0, BOARD_HEIGHT*BOARD_WIDTH), self.playerB.numBlocks))
        if randomPlaceB.shape[0] > 0:
            self.gameState[randomPlaceB // BOARD_WIDTH, randomPlaceB % BOARD_WIDTH, 31] = 1
        #Blocks on board vert
        self.gameState[0:BOARD_HEIGHT-1, 0:BOARD_WIDTH-1, 39] = 1-self.availBlockVertPlace.reshape((BOARD_HEIGHT-1, BOARD_WIDTH-1))
        #Blocks on board horiz
        self.gameState[0:BOARD_WIDTH-1, 0:BOARD_HEIGHT-1, 47] = 1-self.availBlockHorizPlace.reshape((BOARD_WIDTH-1, BOARD_HEIGHT-1))
        #has been repeated
        self.gameState[:, :, 55] = self.pastStates[self.calcStateHash()] == 2
        self.gameState[:, :, 63] = self.pastStates[self.calcStateHash()] == 3
        #whose turn
        if self.currPlayer == self.playerA:
            self.gameState[:,:,64] = 1
        else:
            self.gameState[:,:,64] = 0
            
    
def dfs(tile, yTarget, visited):
    if (tile.yPos == yTarget):
        return True
    
    if (visited[tile.yPos, tile.xPos]):
         return False
        
    visited[tile.yPos, tile.xPos] = True;
    for neighbourTile in tile.neighbours:
        if dfs(neighbourTile, yTarget, visited):
            return True
    return False

def initializeBoard(boardHeight, boardWidth):
    board = np.empty((boardHeight, boardWidth), dtype = Tile)
    for i in range(boardHeight):
        for j in range(boardWidth):
            board[i, j] = Tile(i, j)
            
    for i in range(boardHeight):
        for j in range(boardWidth):
            board[i, j].initNeighbours(board, boardHeight, boardWidth)
                                       
    return board
    
def copyBoard(cloneFrom):
    board = np.empty((cloneFrom.shape[0], cloneFrom.shape[1]), dtype = Tile)
    for i in range(cloneFrom.shape[0]):
        for j in range(cloneFrom.shape[1]):
            board[i, j] = Tile(i, j)
    
    for i in range(cloneFrom.shape[0]):
        for j in range(cloneFrom.shape[1]):
            board[i, j].copyNeighbours(board, cloneFrom[i, j])
    return board

def pickUpBlock(tile0, tile1, tile2, tile3):
    tile0.neighbours.add(tile1)
    tile1.neighbours.add(tile0)
    tile2.neighbours.add(tile3)
    tile3.neighbours.add(tile2)
    
    