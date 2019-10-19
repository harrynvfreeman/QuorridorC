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

extern const int hash_size;

extern const int NUM_BLOCKS;

extern const double e;
extern const double Cpuct;

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
	
	//from Actions
	//treating this as the child
	double N;
	double W;
	double Q;
	double P;
	double vLoss; //vLoss leading to this
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
Node * createChild(Node * parent, QE * state, int move, double p);
Node * selectMCTS(Node * rootNode);
void expandAndEvaluate(Node * node, double * p);
void backup(Node * origNode, double v);
void backupCython(Node * origNode, double * v);
void clearNode(Node * node);
void clearNodeSingle(Node * node);
void play(Tree * tree);

//from play
void search(int numSimulations, Tree * tree);
void selfPlay(int numSimulations, Tree * tree);
void searchCython(int numSimulations, Tree * tree, int * gameState, double * v, double * p, int * isCReady, int * isModelReady, int * error);
void selfPlayCython(int numSimulations, int * gameState, double * v, double * p, int * isCReady, int * isModelReady, int * error);

void cFunctionWorking(int threadNum, int * val, int * wait);

#endif