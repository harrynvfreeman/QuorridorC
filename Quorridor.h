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

extern const float e;
extern const float Cpuct;

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

//from MCTS.c
Node * createNode(QE * state);
Node * createChild(Node * parent, QE * state, int move, float p);
Node * select(Node * rootNode);
void expandAndEvaluate(Node * node, float * p);
void backup(Node * origNode, float v);

//from play
void search(int numSimulations, Tree * tree);

#endif