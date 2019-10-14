class Tile():
    def __init__(self, yPos, xPos):
        self.yPos = yPos
        self.xPos = xPos
        self.neighbours = None
        
    def initNeighbours(self, board, boardHeight, boardWidth):
        xMin = max(self.xPos - 1, 0)
        yMin = max(self.yPos -1 , 0)
            
        xMax = min(self.xPos + 1, boardWidth - 1)
        yMax = min(self.yPos + 1, boardHeight - 1)
            
        self.neighbours = set()
        
        if (self.yPos > 0):
            self.neighbours.add(board[self.yPos-1, self.xPos])
        
        if (self.xPos > 0):
            self.neighbours.add(board[self.yPos, self.xPos-1])
            
        if (self.yPos < boardHeight-1):
            self.neighbours.add(board[self.yPos+1, self.xPos])
        
        if (self.xPos < boardWidth-1):
            self.neighbours.add(board[self.yPos, self.xPos+1])
            
    def copyNeighbours(self, board, cloneFrom):
        self.neighbours = set()
        for cloneTile in cloneFrom.neighbours:
            self.neighbours.add(board[cloneTile.yPos, cloneTile.xPos])
        