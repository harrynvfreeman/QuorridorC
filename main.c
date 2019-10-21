#include "Structs.h"
#include "Quorridor.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h> 

//gcc -o main main.c HashMap.c Player.c Tile.c QuorridorEnvironment.c Utils.c MCTS.c Play.c
//./main

int testDepth(Node * node, int depth);
int max(int a, int b);

const int BOARD_HEIGHT = 9;
const int BOARD_WIDTH = 9;
//const int hash_size = (5+2*(BOARD_HEIGHT-1)*(BOARD_WIDTH-1));
const int hash_size = 133;
//const int NUM_MOVES = 12 + 2*(BOARD_HEIGHT-1)*(BOARD_WIDTH-1);
const int NUM_MOVES = 140;
const int MAX_Y = 7;
const int MIN_Y = 2;
const int MAX_X = 7;
const int MIN_X = 2;
const int NUM_CHANNELS = 29;
const int NUM_ROWS = 17;
const int NUM_COLS = 17;
const int NUM_BLOCKS = 5;
const int MAX_TURNS = 50;
const int BATCH_SIZE = 16;
const double e = 0.25;
//const double e = 0;
const double Cpuct = 1.5;

int mapTest();
Player * playerTest();
Tile * tileTest();
QE * qeTest0(); 
QE * qeTest1();
QE * qeTest2(); 
QE * qeTest3();
void nodeTest0();
void nodeTest1();
void playTest0();
void selfPlayTest0();

int main() {
	srand(time(NULL));
	selfPlayTest0();
	//printf("%d \n", tile->yPos);
	printf("Hello \n");
}

void selfPlayTest0() {
	QE * qe = resetQE();
	Tree * tree = (Tree*)malloc(sizeof(Tree));
	tree->rootNode = createNode(qe);
	selfPlay(400, tree);
}

void playTest0() {
	QE * qe = resetQE();
	Tree * tree = (Tree*)malloc(sizeof(Tree));
	tree->rootNode = createNode(qe);
	search(800, tree);
	
	Node * testNode = tree->rootNode;
	printf("Depth is: %d \n", testDepth(testNode, 0));
	printf("NumChildren is %d \n", testNode->numChildren);
	printf("Parent N is %f \n", testNode->N);
	
	for (int i = 0; i < testNode->numChildren; i++) {
		printf("Wab: %f, %f, %f, %d, \n", 
		(*(testNode->children+i))->N, (*(testNode->children+i))->P, (*(testNode->children+i))->Q, (*(testNode->children+i))->move);
	}
	
	// render(testNode->state, 1);
// 	int r;
// 	while(testNode->numChildren > 0) {
// 		r = rand() % (testNode->numChildren);
// 		testNode = *(testNode->children + r);
// 		render(testNode->state, 1);
// 	}
}

int testDepth(Node * node, int depth) {
	if (node->numChildren == 0) {
		return depth;
	}
	
	int maxim = 0;
	for (int i = 0; i < node->numChildren; i++) {
		maxim = max(maxim, testDepth(*(node->children + i), depth + 1));
	}
	
	return maxim;
}

int max(int a, int b) {
	if (a >= b) {
	 	return a;
	}
	
	return b;
}

void nodeTest0() {
	Node * root = createNode(NULL);
	Node * out = selectMCTS(root);
	
	if (out == root) {
		printf("Success \n");
	} else {
		printf("Fail \n");
	}
}

void nodeTest1() {
	//Works with Cpuct as 2
	Node * root = createNode(NULL);
	Node * child0 = createChild(root, NULL, 0, .5);
	Node * child1 = createChild(root, NULL, 1, .25);
	Node * child2 = createChild(root, NULL, 2, .25);
	
	root->N = 10;
	child0->N = 6;
	child1->N = 2;
	child2->N = 2;
	
	child0->Q = .5;
	child1->Q = 1.5;
	child2->Q = 1;
	
	child0->W = 3;
	child1->W = 3;
	child2->W = 2;
	
	Node * out = selectMCTS(root);
	
	if (out == child0 && out->vLoss == 1 && out->Q == (double)2/7) {
		printf("Success 0 \n");
	} else if (out == child1 && out->vLoss == 1 && out->Q == (double)2/3) {
		printf("Success 1 \n");
	} else if (out == child2 && out->vLoss == 1 && out->Q == (double)1/3) {
		printf("Success 2 \n");
	} else {
		printf("%f \n", out->Q);
		printf("Fail \n");
	}
}

int mapTest() {
	HashMap * map = createHashMap();
	int * testHash = (int*)calloc(hash_size, sizeof(int));
	int * testHash2 = (int*)calloc(hash_size, sizeof(int));
	*(testHash2) = 8;
	insert(map, testHash, 7);
	insert(map, testHash2, 9);
	//return memcmp(testHash, testHash, hash_size);
	//empty(map);
	int out = lookup(map, testHash) + lookup(map, testHash2);
	free(testHash);
	free(map);
	return out;
}

Player * playerTest() {
	Player * player = initPlayer(1,2,8,10,1);
	return player;
}

Tile * tileTest() {
	Tile * tile = initTile(6, 6, MIN_Y, MIN_Y, MAX_Y, MAX_X);
	return tile;
}


//original step and board comparison
QE * qeTest0() {
	QE * qe = resetQE(); 
	
	step(qe, 0);
	step(qe, 2);
	step(qe, 0);
	step(qe, 30);
	step(qe, 103);
	//step(qe, 2);
	//printf("%d \n", posToValVertBlock(MIN_Y, MIN_X, BOARD_WIDTH));
	int * gameState = qe->gameState;
	for (int i = 16; i >= 0; i --) {
		for (int j = 0; j < 17; j++) {
			printf("  %d  ", *(gameState + xyzToVal(NUM_ROWS, NUM_COLS, 2, i, j)));
		}
		printf("\n");
	}
	
	printf("----------------------------------\n");
	for (int i = 16; i >= 0; i = i-2) {
		for (int j = 0; j < 17; j = j+2) {
			printf("  %d  ", *(gameState + xyzToVal(NUM_ROWS, NUM_COLS, 0, i, j)) +
			*(gameState + xyzToVal(NUM_ROWS, NUM_COLS, 1, i, j)));
		}
		printf("\n");
	}
	
	for (int i = 0; i < 200; i++) {
		if (validate(qe, i) == 1) {
			printf("Valid move: %d \n", i);
		}
	}
	
	return qe;
}


QE * qeTest1() {
	QE * qe = resetQE(); 
	
	int i = 0;
	int protectCount = 0;
	int r;
	while(i < 100 && qe->isGameOver == 0 && protectCount < 10000) {
		protectCount = protectCount + 1;
		r = rand() % 140;
		//r = rand() % 4;
		if (validate(qe, r) == 1) {
			step(qe, r);
			i = i + 1;
			printf("played: %d \n", r);
		}
	}
	
	if (qe->isGameOver == 1) {
		printf("winner is: %d \n", qe->winner);
		printf("%d \n", lookup(qe->pastStates, qe->hash));
	}
	
	printf("Player is: %d \n", qe->currPlayer->winVal);
	
	printf("Player A has blockNum: %d \n", qe->playerA->numBlocks);
	
	printf("Player B has blockNum: %d \n", qe->playerB->numBlocks);
	
	int * gameState = qe->gameState;
	for (int i = 16; i >= 0; i --) {
		for (int j = 0; j < 17; j++) {
			printf("  %d  ", *(gameState + xyzToVal(NUM_ROWS, NUM_COLS, 2, i, j)));
		}
		printf("\n");
	}
	
	printf("----------------------------------\n");
	for (int i = 16; i >= 0; i = i-2) {
		for (int j = 0; j < 17; j = j+2) {
			if (*(gameState + xyzToVal(NUM_ROWS, NUM_COLS, 0, i, j)) == 1) {
				printf("  1  ");
			} else if (*(gameState + xyzToVal(NUM_ROWS, NUM_COLS, 1, i, j)) == 1) {
				printf(" -1  ");
			} else {
				printf("  0  ");
			}
		}
		printf("\n");
	}
	
	for (int i = 0; i < 200; i++) {
		if (validate(qe, i) == 1) {
			printf("Valid move: %d \n", i);
		}
	}
	
	return qe;
}

//debugging map
QE * qeTest2() {
	QE * qe = resetQE(); 
	
	step(qe, 3);
	step(qe, 1);
	//step(qe, 2);
	
	MapNode * temp = qe->pastStates->list;
	
	while(temp != NULL) {
		printf("%d, ", temp->val);
		for (int h = 0; h < hash_size; h++) {
			printf("%d", *(temp->key + h));
		}
		printf("\n");
		temp = temp->next;
	}
	
	return qe;
}

QE * qeTest3() {
	QE * qe = resetQE(); 
	
	int i = 0;
	int protectCount = 0;
	int r;
	srand(time(NULL));
	while(i < 100 && qe->isGameOver == 0 && protectCount < 10000) {
		if(qe->playerA->xPos > MAX_X - 1 || qe->playerB->xPos > MAX_X - 1) {
			printf("ERROR \n");
			return qe;
		}
		if(qe->playerA->xPos < MIN_X || qe->playerB->xPos < MIN_X) {
			printf("ERROR \n");
			return qe;
		}
		protectCount = protectCount + 1;
		r = rand() % 140;
		//r = rand() % 4;
		if (validate(qe, r) == 1) {
			step(qe, r);
			i = i + 1;
			char * rend = render(qe, 1);
			free(rend);
			printf("Move was: %d \n", r);
			printf("------------------------------------- \n");
		}
	}
	
	if (qe->isGameOver == 1) {
		printf("Winner is: %d \n", qe->winner);
	} else {
		printf("No winner \n");
	}
	
	return qe;
}

