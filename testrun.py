from QuorridorEnvironment import QuorridorEnvironment as QE
from MCTS import Tree, Node
import Thread
import time 
start = time.time()
env = QE()
env.reset()
rootNode = Node(env)
Thread.search(8,32, rootNode)
end = time.time()
print('Total Time: ' + str(end-start))