def posToValTile(yPos, xPos, boardWidth):
    return xPos + yPos*boardWidth

def valToPosTile(val, boardWidth):
    xPos = val % boardWidth
    yPos = val // boardWidth
    return [yPos, xPos]

def valToPosVertBlock(val, boardWidth):
    y0 = val // (boardWidth - 1)
    x0 = val % (boardWidth - 1)
    y1 = 1 + y0
    x1 = 1 + x0
    return y0, x0, y1, x1
    
def posToValVertBlock(y0, x0, boardWidth):
    return x0 + y0*(boardWidth - 1)

def valToPosHorizBlock(val, boardHeight):
    x0 = val // (boardHeight - 1)
    y0 = val % (boardHeight - 1)
    x1 = 1 + x0
    y1 = 1 + y0
    return y0, x0, y1, x1
    
def posToValHorizBlock(y0, x0, boardHeight):
    return y0 + x0*(boardHeight - 1)

def displayRender(render):
    print('\n'.join([''.join(['{:4}'.format(item) for item in row]) for row in render]))