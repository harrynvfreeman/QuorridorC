#include "Structs.h"
#include "Quorridor.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h> 

//gcc -o main main.c HashMap.c Player.c Tile.c QuorridorEnvironment.c Utils.c
//./main

const int BOARD_HEIGHT = 9;
const int BOARD_WIDTH = 9;
const int hash_size = (5+2*(BOARD_HEIGHT-1)*(BOARD_WIDTH-1));
const int MAX_Y = 7;
const int MIN_Y = 2;
const int MAX_X = 7;
const int MIN_X = 2;
const int NUM_CHANNELS = 29;
const int NUM_ROWS = 17;
const int NUM_COLS = 17;
const int NUM_BLOCKS = 5;

int mapTest();
Player * playerTest();
Tile * tileTest();
QE * qeTest0(); 
QE * qeTest1();
QE * qeTest2(); 
QE * qeTest3();

int main() {
	QE * qe = qeTest3();
	//printf("%d \n", tile->yPos);
	printf("Hello \n");
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

