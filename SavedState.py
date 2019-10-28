from pickle import dump, load
class SavedState():
    #def __init__(self, gameState, pi, value, env):
    def __init__(self, gameState, pi, value):
        self.gameState = gameState
        self.pi = pi
        self.value = value
        #self.env = env
        
class SavedStateTwo():
    def __init__(self, gameState, piType, piMove, piBlock, value):
        self.gameState = gameState
        self.piType = piType
        self.piMove = piMove
        self.piBlock = piBlock
        self.value = value
        
class VersionDescriptor():
    def __init__(self, version):
        self.version = version
        
def readSavedState(path):
    return load(open(path, 'rb'))

def writeSavedState(savedState, path):
    dump(savedState, open(path, 'wb'))