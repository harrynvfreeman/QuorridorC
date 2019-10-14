from distutils.core import setup
from Cython.Build import cythonize
from distutils.extension import Extension
import numpy

#setup(ext_modules = cythonize('example_cy.pyx'))
#setup(ext_modules = cythonize('createHogFeatures36C.pyx'))
#setup(ext_modules = cythonize('nmsHOGC.pyx'))
extensions = [Extension('SelfPlayC', ['SelfPlay.pyx', 'Play.c', 'QuorridorEnvironment.c', 'Player.c', 'Tile.c', 'main.c', 'HashMap.c', 'Utils.c', 'MCTS.c'], depends=['Structs.h', 'Quorridor.h'])]
setup(ext_modules = cythonize(extensions), include_dirs=[numpy.get_include()])