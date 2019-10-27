#ifndef quorridor
#define quorridor

//total board height
extern const int BOARD_HEIGHT;
//total board width
extern const int BOARD_WIDTH;

//max's are exclusive, min's are inclusive
extern const int MAX_Y;
extern const int MAX_X;
extern const int MIN_Y;
extern const int MIN_X;
extern const int NUM_CHANNELS;
extern const int NUM_ROWS;
extern const int NUM_COLS;
extern const int NUM_MOVES;
extern const int MAX_TURNS;
extern const int BATCH_SIZE;

extern const int hash_size;

extern const int NUM_BLOCKS;

extern const float e;
extern const float Cpuct;

struct timespec;

//structs
typedef struct Player Player;
typedef struct Tile Tile;
typedef struct QE QE;
typedef struct HashMap HashMap;
typedef struct MapNode MapNode;
typedef struct Node Node;
typedef struct Tree Tree;

struct Player {
	int yPos;
	int xPos;
	int yTarget;
	int numBlocks;
	int winVal;
};

struct Tile {
	int yPos;
	int xPos;
	int neighbourUp;
	int neighbourRight;
	int neighbourDown;
	int neighbourLeft;
};

struct QE {
	Player * playerA;
	Player * playerB;
	Tile *** board;
	Player * currPlayer;
	Player * nextPlayer;
	int * availBlockVertPlace;
	int * availBlockHorizPlace;
	int * placedVertBlocks;
	int * placedHorizBlocks;
	int turnNum; 
	int * gameState;
	int * hash;
	HashMap * pastStates;
	int winner;
	int isGameOver;
};

//http://www.kaushikbaruah.com/posts/data-structure-in-c-hashmap/
//make sure to reference above
struct MapNode {
	int * key;
	int val;
	MapNode * next;
};

struct HashMap {
	MapNode * list;
};

struct Node {
	//from Node
	QE * state;
	Node ** children;
	int numChildren;
	Node * parent;
	int hasParent;
	float * pi;
	
	//from Actions
	//treating this as the child
	float N;
	float W;
	float Q;
	float P;
	float vLoss; //vLoss leading to this
	//Node * parent;
	int move; //move that lead to this
};

struct Tree {
	Node * rootNode;
};

//from HashMap.c
HashMap* createHashMap();
void insert(HashMap * map, int * key, int val);
int lookup(HashMap * map, int * key);
void empty(HashMap * map);
HashMap * cloneMap(HashMap * cloneFrom);

//from Utils.c
int posToValTile(int yPos, int xPos, int boardWidth);
void valToPosTile(int * outPos, int val, int boardWidth);
void valToPosVertBlock(int * outPos, int val, int boardWidth);
int posToValVertBlock(int y0, int x0, int boardWidth);
void valToPosHorizBlock(int * outPos, int val, int boardHeight);
int posToValHorizBlock(int y0, int x0, int boardHeight);
int xyzToVal(int rows, int cols, int z, int y, int x);

//from Tile.c
Tile * initTile(int yPos, int xPos, int minY, int minX, int maxY, int maxX);
Tile * cloneTile(Tile * cloneFrom);
Player * clonePlayer(Player * cloneFrom);

//from Player.c
Player * initPlayer(int yPos, int xPos, int yTarget, int numBlocks, int winVal);

//from QuorridorEnvironment.c
QE * resetQE();
// QE * initQE(Player * playerA, Player * playerB, Tile *** board, Player * currPlayer,
// 			Player * nextPlayer, int * availBlockVertPlace, int * availBlockHorizPlace,
// 			int * placedVertBlocks, int * placedHorizBlocks, int turnNum, int * gameState,
// 			int * hash, HashMap * pastStates, int * winner);

int validate(QE * qe, int action);
int step(QE * qe, int action);
int * calcStateHash(QE * qe);
char * render(QE * qe, int display);
QE * cloneQE(QE * cloneFrom);
void clearQE(QE * qe);

//from MCTS.c
Node * createNode(QE * state);
Node * createChild(Node * parent, QE * state, int move, float p);
Node * selectMCTS(Node * rootNode, int * numChildren, float * dirichlet, 
				int * diriCReady, int * diriModelReady, struct timespec * tm1, struct timespec * tm2);
void expandAndEvaluate(Node * node, float * pType, float * pMove, float * pBlock);
void backup(Node * origNode, float v);
void backupCython(Node * origNode, float * v, int shouldUpdateN);
void clearNode(Node * node);
void clearNodeSingle(Node * node);
void play(Tree * tree, float * pRChoice, int * indRChoice, int * rChoiceReadyC, int * rChoiceReadyModel);
void playHuman(Tree * tree, int move);
void playAgainstHuman(Tree * tree);

//from play
//void search(int numSimulations, Tree * tree);
//void selfPlay(int numSimulations, Tree * tree);
void playMatchCython(int numSimulations, int * gameState, float * v, float * p, 
					int * isCReady, int * isModelReady, 
					int * isCReadyForHuman, int * isHumanReady, int * humanMove,
					int * error);
void selfPlayCython(int numSimulations, int * gameState, float * v, 
					float * pType, float * pMove, float * pBlock, 
					int * isCReady, int * isModelReady, 
					int * numTurns, int * gameStateOut, float * vOut, float * piOut,
					float * pRChoice, int * indRChoice, int * rChoiceReadyC, int * rChoiceReadyModel,
					int * numChildren, float * dirichlet, int * diriCReady, int * diriModelReady,
					int * error);

void cFunctionWorking(int threadNum, int * val, int * wait);

//from random_real
float
random_real(void);

#endif