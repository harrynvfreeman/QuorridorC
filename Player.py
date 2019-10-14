class Player:
    def __init__(self, yPos, yTarget, xPos, numBlocks, name):
        self.xPos = xPos
        self.yPos = yPos
        self.yTarget = yTarget
        self.numBlocks = numBlocks
        self.nextPlayer = None
        self.name = name