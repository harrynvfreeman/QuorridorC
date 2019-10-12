#ifndef structs
#define structs

typedef struct Player Player;
typedef struct Tile Tile;
typedef struct QE QE;
typedef struct HashMap HashMap;
typedef struct MapNode MapNode;
typedef struct Node Node;
// typedef struct Edge Edge;
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
	float N;
	float W;
	float Q;
	float P;
	float vLoss; //vLoss leading to this
	//Node * parent;
	int move; //move that lead to this
};

// struct Edge {
// 	float * N;
// 	float * W;
// 	float * Q;
// 	float * P;
// 	float * vLoss;
// 	Node * child;
// 	Node * parent;
// 	int * move;
// };

struct Tree {
	Node * rootNode;
};


#endif
