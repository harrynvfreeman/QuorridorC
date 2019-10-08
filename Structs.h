#ifndef structs
#define structs

typedef struct Player Player;
typedef struct Tile Tile;
typedef struct QE QE;
typedef struct HashMap HashMap;
typedef struct MapNode MapNode;

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


#endif
