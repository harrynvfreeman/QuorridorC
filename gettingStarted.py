# from QuorridorEnvironment import QuorridorEnvironment as QE
# from MCTS import Tree, Node
# import Thread
# import time
# 
# start = time.time()
# env = QE()
# env.reset() 
# tree = Tree(Node(env))
# Thread.search(1,100, tree)
# end = time.time()
# 
# print('Total Time: ' + str(end-start))
from multiprocessing import Process, Pool, Lock, Manager
from multiprocessing.managers import BaseManager

class TestClass():
    def __init__(self):
        self.count = 0
        self.lock = Lock()
        self.child = None
        
    def increment(self, l, i):
        self.lock.acquire()
        l[i] = i
        node = self
        node.count = node.count + 1
        while(node.child):
            node = node.child
            node.count = node.count + 1
        node.child = TestClass()
        self.lock.release()
        
    def get(self):
        return self.count
    
    def getSelf(self):
        return self
    
    def getChild(self):
        return self.child.child.child.get()
    
BaseManager.register('TestClass', TestClass)

if __name__ == '__main__':
    manager = BaseManager()
    manager.start()
    shared = manager.TestClass()
    
    secondManager = Manager()
    returnThing = secondManager.list([None]*3)
    
    process0 = Process(target=shared.increment, args=(returnThing, 0))
    process1 = Process(target=shared.increment, args=(returnThing, 1))
    process2 = Process(target=shared.increment, args=(returnThing, 2))
    
    process0.start()
    process1.start()
    process2.start()
    
    process0.join()
    process1.join()
    process2.join()

    # pool = Pool(5)
    # for i in range(33):
    #     pool.apply(func=shared.increment)
    # pool.close()
    # pool.join()
    print (str(shared.get()))
    print (str(shared.getChild()))
    print(str(returnThing))
    #print(str(returnThing))