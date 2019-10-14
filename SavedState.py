from pickle import dump, load
class SavedState():
    def __init__(self, gameState, pi, value, env):
        self.gameState = gameState
        self.pi = pi
        self.value = value
        self.env = env
        
def readSavedState(path):
    return load(open(path, 'rb'))

def writeSavedState(savedState, path):
    dump(savedState, open(path, 'wb'))