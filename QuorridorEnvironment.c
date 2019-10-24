#include "Structs.h"
#include "Quorridor.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

Tile *** initBoard();
void initAvailBlockPlace(int * availBlockVertPlace, int * availBlockHorizPlace);
int isMovePlayerUpValid(QE * qe);
void movePlayerUp(QE * qe);
int isMovePlayerDownValid(QE * qe);
void movePlayerDown(QE * qe);
int isMovePlayerRightValid(QE * qe);
void movePlayerRight(QE * qe);
int isMovePlayerLeftValid(QE * qe);
void movePlayerLeft(QE * qe);
int isMovePlayerTwoUpValid(QE * qe);
void movePlayerTwoUp(QE * qe);
int isMovePlayerTwoDownValid(QE * qe);
void movePlayerTwoDown(QE * qe);
int isMovePlayerTwoRightValid(QE * qe);
void movePlayerTwoRight(QE * qe);
int isMovePlayerTwoLeftValid(QE * qe);
void movePlayerTwoLeft(QE * qe);
int isMovePlayerUpRightValid(QE * qe);
int isMovePlayerRightUpValid(QE * qe);
void movePlayerUpRight(QE * qe);
int isMovePlayerDownRightValid(QE * qe);
int isMovePlayerRightDownValid(QE * qe);
void movePlayerDownRight(QE * qe);
int isMovePlayerDownLeftValid(QE * qe);
int isMovePlayerLeftDownValid(QE * qe);
void movePlayerDownLeft(QE * qe);
int isMovePlayerUpLeftValid(QE * qe);
int isMovePlayerLeftUpValid(QE * qe);
void movePlayerUpLeft(QE * qe);
int isPlaceVerticalBlockValid(QE * qe, int m);
void placeVerticalBlock(QE * qe, int m);
void putVertBlock(int * outPos, QE * qe, int m);
void pickUpVertBlock(int * outPos, QE * qe, int m);
int isPlaceHorizontalBlockValid(QE * qe, int m);
void placeHorizontalBlock(QE * qe, int m);
void putHorizBlock(int * outPos, QE * qe, int m);
void pickUpHorizBlock(int * outPos, QE * qe, int m);
int dfs(Tile * tile, Tile *** board, int yTarget, int ** visited, int action);
void updateGameState(QE * qe);
void renderTileRow(QE* qe, char * rend, int row);
void renderBlockRow(QE * qe, char * rend, int row);
Tile *** cloneBoard(Tile *** cloneFrom);
void clearBoard(Tile *** board);


//currPlayer 1 is playerA, -1 playerB
//winner 1 is playerA, -1 playerB, 0 draw
QE * resetQE() {
	QE * qe = (QE*)malloc(sizeof(QE));
	Player * playerA = initPlayer(MIN_Y, (MIN_X + MAX_X)/2, MAX_Y - 1, NUM_BLOCKS, 1);
	Player * playerB = initPlayer(MAX_Y - 1, (MIN_X + MAX_X)/2, MIN_Y, NUM_BLOCKS, -1);
	Tile *** board = initBoard();
	int * availBlockVertPlace = (int*)calloc((BOARD_HEIGHT-1)*(BOARD_WIDTH-1), sizeof(int));
	int * availBlockHorizPlace = (int*)calloc((BOARD_HEIGHT-1)*(BOARD_WIDTH-1), sizeof(int));
	initAvailBlockPlace(availBlockVertPlace, availBlockHorizPlace);
	int * placedVertBlocks = (int*)calloc((BOARD_HEIGHT-1)*(BOARD_WIDTH-1), sizeof(int));
	int * placedHorizBlocks = (int*)calloc((BOARD_HEIGHT-1)*(BOARD_WIDTH-1), sizeof(int));
	int turnNum = 0;
	int winner = 0;
	int isGameOver = 0;
	int * gameState = (int*)malloc((NUM_CHANNELS*NUM_ROWS*NUM_COLS)*sizeof(int));
	
	qe->playerA = playerA;
	qe->playerB = playerB;
	qe->currPlayer = playerA;
	qe->nextPlayer = playerB;
	qe->board = board;
	qe->availBlockVertPlace = availBlockVertPlace;
	qe->availBlockHorizPlace = availBlockHorizPlace;
	qe->placedVertBlocks = placedVertBlocks;
	qe->placedHorizBlocks = placedHorizBlocks;
	qe->turnNum = turnNum;
	qe->winner = winner;
	qe->isGameOver = isGameOver;
	qe->hash = calcStateHash(qe);
	qe->pastStates = createHashMap();
	insert(qe->pastStates, qe->hash, 1);
	qe->gameState = gameState;
	updateGameState(qe);
	
	return qe;
}

QE * cloneQE(QE * cloneFrom) {
	QE * qe = (QE*)malloc(sizeof(QE));
	Player * playerA = clonePlayer(cloneFrom->playerA);
	Player * playerB = clonePlayer(cloneFrom->playerB);
	Player * currPlayer;
	Player * nextPlayer;
	if (cloneFrom->currPlayer->winVal == playerA->winVal) {
		currPlayer = playerA;
		nextPlayer = playerB;
	} else {
		currPlayer = playerB;
		nextPlayer = playerA;
	}
	Tile *** board = cloneBoard(cloneFrom->board);
	int * availBlockVertPlace = (int*)malloc((BOARD_HEIGHT-1)*(BOARD_WIDTH-1)*sizeof(int));
	int * availBlockHorizPlace = (int*)malloc((BOARD_HEIGHT-1)*(BOARD_WIDTH-1)*sizeof(int));
	memcpy(availBlockVertPlace, cloneFrom->availBlockVertPlace, (BOARD_HEIGHT-1)*(BOARD_WIDTH-1)*sizeof(int));
	memcpy(availBlockHorizPlace, cloneFrom->availBlockHorizPlace, (BOARD_HEIGHT-1)*(BOARD_WIDTH-1)*sizeof(int));
	int * placedVertBlocks = (int*)malloc((BOARD_HEIGHT-1)*(BOARD_WIDTH-1)*sizeof(int));
	int * placedHorizBlocks = (int*)malloc((BOARD_HEIGHT-1)*(BOARD_WIDTH-1)*sizeof(int));
	memcpy(placedVertBlocks, cloneFrom->placedVertBlocks, (BOARD_HEIGHT-1)*(BOARD_WIDTH-1)*sizeof(int));
	memcpy(placedHorizBlocks, cloneFrom->placedHorizBlocks, (BOARD_HEIGHT-1)*(BOARD_WIDTH-1)*sizeof(int));
	int turnNum = cloneFrom->turnNum;
	int winner = cloneFrom->winner;
	int isGameOver = cloneFrom->isGameOver;
	int * gameState = (int*)malloc((NUM_CHANNELS*NUM_ROWS*NUM_COLS)*sizeof(int));
	memcpy(gameState, cloneFrom->gameState, (NUM_CHANNELS*NUM_ROWS*NUM_COLS)*sizeof(int));
	
	qe->playerA = playerA;
	qe->playerB = playerB;
	qe->currPlayer = currPlayer;
	qe->nextPlayer = nextPlayer;
	qe->board = board;
	qe->availBlockVertPlace = availBlockVertPlace;
	qe->availBlockHorizPlace = availBlockHorizPlace;
	qe->placedVertBlocks = placedVertBlocks;
	qe->placedHorizBlocks = placedHorizBlocks;
	qe->turnNum = turnNum;
	qe->winner = winner;
	qe->isGameOver = isGameOver;
	qe->hash = calcStateHash(qe);
	qe->pastStates = cloneMap(cloneFrom->pastStates);
	qe->gameState = gameState;
	
	return qe;
}

void clearQE(QE * qe) {
	free(qe->playerA);
	qe->playerA = NULL;
	free(qe->playerB);
	qe->playerB = NULL;
	clearBoard(qe->board);
	free(qe->availBlockVertPlace);
	qe->availBlockVertPlace = NULL;
	free(qe->availBlockHorizPlace);
	qe->availBlockHorizPlace = NULL;
	free(qe->placedVertBlocks);
	qe->placedVertBlocks = NULL;
	free(qe->placedHorizBlocks);
	qe->placedHorizBlocks = NULL;
	free(qe->hash);
	qe->hash = NULL;
	empty(qe->pastStates);
	free(qe->pastStates);
	qe->pastStates = NULL;
	free(qe->gameState);
	qe->gameState = NULL;
	
	free(qe);
	qe = NULL;
}

Tile *** initBoard() {
	Tile *** board = (Tile***)malloc(BOARD_HEIGHT*sizeof(Tile**));
	for (int i = 0; i < BOARD_HEIGHT; ++i) {
		Tile** iBoard = (Tile**)malloc(BOARD_WIDTH*sizeof(Tile*));
		for (int j = 0; j < BOARD_WIDTH; ++j) {
			*(iBoard + j) = initTile(i, j, MIN_Y, MIN_X, MAX_Y, MAX_X);
		}
		
		*(board + i) = iBoard;
	}
	
	return board;
}

Tile *** cloneBoard(Tile *** cloneFrom) {
	Tile *** board = (Tile***)malloc(BOARD_HEIGHT*sizeof(Tile**));
	for (int i = 0; i < BOARD_HEIGHT; ++i) {
		Tile** iBoard = (Tile**)malloc(BOARD_WIDTH*sizeof(Tile*));
		for (int j = 0; j < BOARD_WIDTH; ++j) {
			Tile * tile = cloneTile(cloneFrom[i][j]);
			*(iBoard + j) = tile;
		}
		
		*(board + i) = iBoard;
	}
	
	return board;
}

void clearBoard(Tile *** board) {
	for (int i = 0; i < BOARD_HEIGHT; ++i) {
		for (int j = 0; j < BOARD_WIDTH; ++j) {
			Tile * tile = board[i][j];
			free(tile);
			tile = NULL;
		}
		free(*(board + i));
		*(board + i) = NULL;
	}
	
	free(board);
	board = NULL;
}

void initAvailBlockPlace(int * availBlockVertPlace, int * availBlockHorizPlace) {
	for (int i = MIN_Y; i < MAX_Y - 1; ++i) {
		for (int j = MIN_X; j < MAX_X - 1; j ++) {
			*(availBlockVertPlace + posToValVertBlock(i, j, BOARD_WIDTH)) = 1;
			*(availBlockHorizPlace + posToValHorizBlock(i, j, BOARD_HEIGHT)) = 1;
		}
	}
}

// QE * initQE(Player * playerA, Player * playerB, Tile *** board, Player * currPlayer,
// 			Player * nextPlayer, int * availBlockVertPlace, int * availBlockHorizPlace,
// 			int * placedVertBlocks, int * placedHorizBlocks, int turnNum, int * gameState,
// 			int * hash, HashMap * pastStates, int winner, int * isGameOver) {
// 	QE * qe = (QE*)malloc(sizeof(QE));
// 	qe->playerA = playerA;
// 	qe->playerB = playerB;
// 	qe->board = board;
// 	qe->currPlayer = currPlayer;
// 	qe->nextPlayer = nextPlayer;
// 	qe->availBlockVertPlace = availBlockVertPlace;
// 	qe->availBlockHorizPlace = availBlockHorizPlace;
// 	qe->placedVertBlocks = placedVertBlocks;
// 	qe->placedHorizBlocks = placedHorizBlocks;
// 	qe->turnNum = turnNum;
// 	qe->gameState = gameState;
// 	qe->hash = hash;
// 	qe->pastStates = pastStates;
// 	qe->winner = winner;
// 	qe->isGameOver = isGameOver;
// 	
// 	return qe;
// }

int validate(QE * qe, int action) {
	if(qe->isGameOver == 1) {
		return 0;
	}

	if (action == 0) {
		return isMovePlayerUpValid(qe);
	} else if (action == 1) {
		return isMovePlayerRightValid(qe);
	} else if (action == 2) {
		return isMovePlayerDownValid(qe);
	} else if (action == 3) {
		return isMovePlayerLeftValid(qe);
	} else if (action == 4) {
		return isMovePlayerTwoUpValid(qe);
	} else if (action == 5) {
		return isMovePlayerTwoRightValid(qe);
	} else if (action == 6) {
		return isMovePlayerTwoDownValid(qe);
	} else if (action == 7) {
		return isMovePlayerTwoLeftValid(qe);
	} else if (action == 8) {
		if (isMovePlayerRightUpValid(qe) == 1) {
			return 1;
		}
		return isMovePlayerUpRightValid(qe);
	} else if (action == 9) {
		if (isMovePlayerRightDownValid(qe) == 1) {
			return 1;
		}
		return isMovePlayerDownRightValid(qe);
	} else if (action == 10) {
		if (isMovePlayerLeftDownValid(qe) == 1) {
			return 1;
		}
		return isMovePlayerDownLeftValid(qe);
	} else if (action == 11) {
		if (isMovePlayerLeftUpValid(qe) == 1) {
			return 1;
		}
		return isMovePlayerUpLeftValid(qe);
	} else if (action >= 12 + posToValVertBlock(MIN_Y, MIN_X, BOARD_WIDTH) &&
		action <= 12 + posToValVertBlock(MAX_Y-2, MAX_X-2, BOARD_WIDTH)) {
		return isPlaceVerticalBlockValid(qe, action - 12);	
	} else if (action >= 12 + (BOARD_HEIGHT-1)*(BOARD_WIDTH-1) + posToValHorizBlock(MIN_Y, MIN_X, BOARD_HEIGHT)
		&& action <= 12 + (BOARD_HEIGHT-1)*(BOARD_WIDTH-1) + posToValHorizBlock(MAX_Y-2, MAX_X-2, BOARD_HEIGHT)) {
		return isPlaceHorizontalBlockValid(qe, action - 12 - (BOARD_HEIGHT-1)*(BOARD_WIDTH-1));	
	} else {
		//return -1;
		return 0;
	}
}

int step(QE * qe, int action) {
	if (action == 0) {
		movePlayerUp(qe);
	} else if (action == 1) {
		movePlayerRight(qe);
	} else if (action == 2) {
		movePlayerDown(qe);
	} else if (action == 3) {
		movePlayerLeft(qe);
	} else if (action == 4) {
		movePlayerTwoUp(qe);
	} else if (action == 5) {
		movePlayerTwoRight(qe);
	} else if (action == 6) {
		movePlayerTwoDown(qe);
	} else if (action == 7) {
		movePlayerTwoLeft(qe);
	} else if (action == 8) {
		movePlayerUpRight(qe);
	} else if (action == 9) {
		movePlayerDownRight(qe);
	} else if (action == 10) {
		movePlayerDownLeft(qe);
	} else if (action == 11) {
		movePlayerUpLeft(qe);
	} else if (action >= 12 + posToValVertBlock(MIN_Y, MIN_X, BOARD_WIDTH) &&
		action <= 12 + posToValVertBlock(MAX_Y-2, MAX_X-2, BOARD_WIDTH)) {
		placeVerticalBlock(qe, action - 12);
		empty(qe->pastStates);
	} else if (action >= 12 + (BOARD_HEIGHT-1)*(BOARD_WIDTH-1) + posToValHorizBlock(MIN_Y, MIN_X, BOARD_HEIGHT)
		&& action <= 12 + (BOARD_HEIGHT-1)*(BOARD_WIDTH-1) + posToValHorizBlock(MAX_Y-2, MAX_X-2, BOARD_HEIGHT)) {
		placeHorizontalBlock(qe, action - 12 - (BOARD_HEIGHT-1)*(BOARD_WIDTH-1));
		empty(qe->pastStates);	
	} else {
		//return -1;
		return 0;
	}
	
	qe->turnNum = qe->turnNum + 1;
	
	if (qe->turnNum >= MAX_TURNS && qe->isGameOver == 0) {
		qe->winner = 0;
		qe->isGameOver = 1;
	}
	
	Player * tempPlayer = qe->currPlayer;
	qe->currPlayer = qe->nextPlayer;
	qe->nextPlayer = tempPlayer;
	
	int * hash = calcStateHash(qe);
	
	free(qe->hash);
		
	qe->hash = hash;
	
	int val = lookup(qe->pastStates, hash);
	
	if (val == -1) {
		insert(qe->pastStates, hash, 1);
	} else if (val == 2) {
		if (qe->isGameOver == 0) {
			qe->winner = 0;
			qe->isGameOver = 1;
		}
		insert(qe->pastStates, hash, 3);
	} else {
		insert(qe->pastStates, hash, 2);
	}
	
	updateGameState(qe);
	
	return 1;
}

//Do this before or after move? had after, is that right?
int * calcStateHash(QE * qe) {
	int * hash = (int*)malloc(hash_size*sizeof(int));
	*(hash) = (qe->currPlayer == qe->playerA) ? 1 : 0;
	*(hash + 1) = qe->playerA->yPos;
	*(hash + 2) = qe->playerA->xPos;
	*(hash + 3) = qe->playerB->yPos;
	*(hash + 4) = qe->playerB->xPos;
	memcpy(hash + 5, qe->availBlockVertPlace, (BOARD_HEIGHT-1)*(BOARD_WIDTH-1)*sizeof(int));
	memcpy(hash + 5 + (BOARD_HEIGHT-1)*(BOARD_WIDTH-1), qe->availBlockHorizPlace, (BOARD_HEIGHT-1)*(BOARD_WIDTH-1)*sizeof(int));
	
	return hash;
}

int isMovePlayerUpValid(QE * qe) {

	Tile * tile = qe->board[qe->currPlayer->yPos][qe->currPlayer->xPos];
	
	if (tile->neighbourUp == 0) {
		return 0;
	}
	
	if (qe->currPlayer->yPos + 1 == qe->nextPlayer->yPos && qe->currPlayer->xPos == qe->nextPlayer->xPos) {
		return 0;
	}
	
	return 1;
}

void movePlayerUp(QE * qe) {
	qe->currPlayer->yPos = qe->currPlayer->yPos + 1;
	if (qe->currPlayer->yPos == qe->currPlayer->yTarget) {
		qe->winner = qe->currPlayer->winVal;
		qe->isGameOver = 1;
	}
}

int isMovePlayerDownValid(QE * qe) {

	Tile * tile = qe->board[qe->currPlayer->yPos][qe->currPlayer->xPos];
	
	if (tile->neighbourDown == 0) {
		return 0;
	}
	
	if (qe->currPlayer->yPos - 1 == qe->nextPlayer->yPos && qe->currPlayer->xPos == qe->nextPlayer->xPos) {
		return 0;
	}
	
	return 1;
}

void movePlayerDown(QE * qe) {
	qe->currPlayer->yPos = qe->currPlayer->yPos - 1;
	if (qe->currPlayer->yPos == qe->currPlayer->yTarget) {
		qe->winner = qe->currPlayer->winVal;
		qe->isGameOver = 1;
	}
}

int isMovePlayerRightValid(QE * qe) {

	Tile * tile = qe->board[qe->currPlayer->yPos][qe->currPlayer->xPos];
	
	if (tile->neighbourRight == 0) {
		return 0;
	}
	
	if (qe->currPlayer->yPos == qe->nextPlayer->yPos && qe->currPlayer->xPos + 1 == qe->nextPlayer->xPos) {
		return 0;
	}
	
	return 1;
}

void movePlayerRight(QE * qe) {
	qe->currPlayer->xPos = qe->currPlayer->xPos + 1;
}

int isMovePlayerLeftValid(QE * qe) {

	Tile * tile = qe->board[qe->currPlayer->yPos][qe->currPlayer->xPos];
	
	if (tile->neighbourLeft == 0) {
		return 0;
	}
	
	if (qe->currPlayer->yPos == qe->nextPlayer->yPos && qe->currPlayer->xPos - 1 == qe->nextPlayer->xPos) {
		return 0;
	}
	
	return 1;
}

void movePlayerLeft(QE * qe) {
	qe->currPlayer->xPos = qe->currPlayer->xPos - 1;
}

int isMovePlayerTwoUpValid(QE * qe) {

	Tile * currTile = qe->board[qe->currPlayer->yPos][qe->currPlayer->xPos];
	
	if (currTile->neighbourUp == 0) {
		return 0;
	}
	
	Tile * midTile = qe->board[qe->currPlayer->yPos + 1][qe->currPlayer->xPos];
	
	if (midTile->neighbourUp == 0) {
		return 0;
	}
	
	if (qe->currPlayer->yPos + 1 == qe->nextPlayer->yPos && qe->currPlayer->xPos == qe->nextPlayer->xPos) {
		return 1;
	}
	
	return 0;
}

void movePlayerTwoUp(QE * qe) {
	qe->currPlayer->yPos = qe->currPlayer->yPos + 2;
	if (qe->currPlayer->yPos == qe->currPlayer->yTarget) {
		qe->winner = qe->currPlayer->winVal;
		qe->isGameOver = 1;
	}
}

int isMovePlayerTwoDownValid(QE * qe) {
	
	Tile * currTile = qe->board[qe->currPlayer->yPos][qe->currPlayer->xPos];
	
	if (currTile->neighbourDown == 0) {
		return 0;
	}
	
	Tile * midTile = qe->board[qe->currPlayer->yPos - 1][qe->currPlayer->xPos];
	
	if (midTile->neighbourDown == 0) {
		return 0;
	}
	
	if (qe->currPlayer->yPos - 1 == qe->nextPlayer->yPos && qe->currPlayer->xPos == qe->nextPlayer->xPos) {
		return 1;
	}
	
	return 0;
}

void movePlayerTwoDown(QE * qe) {
	qe->currPlayer->yPos = qe->currPlayer->yPos - 2;
	if (qe->currPlayer->yPos == qe->currPlayer->yTarget) {
		qe->winner = qe->currPlayer->winVal;
		qe->isGameOver = 1;
	}
}

int isMovePlayerTwoRightValid(QE * qe) {

	Tile * currTile = qe->board[qe->currPlayer->yPos][qe->currPlayer->xPos];
	
	if (currTile->neighbourRight == 0) {
		return 0;
	}
	
	Tile * midTile = qe->board[qe->currPlayer->yPos][qe->currPlayer->xPos + 1];
	
	if (midTile->neighbourRight == 0) {
		return 0;
	}
	
	if (qe->currPlayer->yPos == qe->nextPlayer->yPos && qe->currPlayer->xPos + 1 == qe->nextPlayer->xPos) {
		return 1;
	}
	
	return 0;
}

void movePlayerTwoRight(QE * qe) {
	qe->currPlayer->xPos = qe->currPlayer->xPos + 2;
}

int isMovePlayerTwoLeftValid(QE * qe) {

	Tile * currTile = qe->board[qe->currPlayer->yPos][qe->currPlayer->xPos];
	
	if (currTile->neighbourLeft == 0) {
		return 0;
	}
	
	Tile * midTile = qe->board[qe->currPlayer->yPos][qe->currPlayer->xPos - 1];
	
	if (midTile->neighbourLeft == 0) {
		return 0;
	}
	
	if (qe->currPlayer->yPos == qe->nextPlayer->yPos && qe->currPlayer->xPos - 1 == qe->nextPlayer->xPos) {
		return 1;
	}
	
	return 0;
}

void movePlayerTwoLeft(QE * qe) {
	qe->currPlayer->xPos = qe->currPlayer->xPos - 2;
}

//Breaking these up, up right is different than right up
//Conditions:
//Can move up one
//Up one is player
//Behind player is block
//Can move left from up one
int isMovePlayerUpRightValid(QE * qe) {

	Tile * currTile = qe->board[qe->currPlayer->yPos][qe->currPlayer->xPos];
	
	if (currTile->neighbourUp == 0) {
		return 0;
	}
	
	if (qe->currPlayer->yPos + 1 != qe->nextPlayer->yPos || qe->currPlayer->xPos != qe->nextPlayer->xPos) {
		return 0;
	}
	
	Tile * midTile = qe->board[qe->currPlayer->yPos + 1][qe->currPlayer->xPos];
	
	if (midTile->neighbourRight == 0 || midTile->neighbourUp == 1) {
		return 0;
	}
	
	return 1;
}

int isMovePlayerRightUpValid(QE * qe) {

	Tile * currTile = qe->board[qe->currPlayer->yPos][qe->currPlayer->xPos];
	
	if (currTile->neighbourRight == 0) {
		return 0;
	}
	
	if (qe->currPlayer->yPos != qe->nextPlayer->yPos || qe->currPlayer->xPos + 1 != qe->nextPlayer->xPos) {
		return 0;
	}
	
	Tile * midTile = qe->board[qe->currPlayer->yPos][qe->currPlayer->xPos + 1];
	
	if (midTile->neighbourUp == 0 || midTile->neighbourRight == 1) {
		return 0;
	}
	
	return 1;
}

void movePlayerUpRight(QE * qe) {
	qe->currPlayer->yPos = qe->currPlayer->yPos + 1;
	qe->currPlayer->xPos = qe->currPlayer->xPos + 1;
	if (qe->currPlayer->yPos == qe->currPlayer->yTarget) {
		qe->winner = qe->currPlayer->winVal;
		qe->isGameOver = 1;
	}
}

int isMovePlayerDownRightValid(QE * qe) {

	Tile * currTile = qe->board[qe->currPlayer->yPos][qe->currPlayer->xPos];
	
	if (currTile->neighbourDown == 0) {
		return 0;
	}
	
	if (qe->currPlayer->yPos - 1 != qe->nextPlayer->yPos || qe->currPlayer->xPos != qe->nextPlayer->xPos) {
		return 0;
	}
	
	Tile * midTile = qe->board[qe->currPlayer->yPos - 1][qe->currPlayer->xPos];
	
	if (midTile->neighbourRight == 0 || midTile->neighbourDown == 1) {
		return 0;
	}
	
	return 1;
}

int isMovePlayerRightDownValid(QE * qe) {

	Tile * currTile = qe->board[qe->currPlayer->yPos][qe->currPlayer->xPos];
	
	if (currTile->neighbourRight == 0) {
		return 0;
	}
	
	if (qe->currPlayer->yPos != qe->nextPlayer->yPos || qe->currPlayer->xPos + 1 != qe->nextPlayer->xPos) {
		return 0;
	}
	
	Tile * midTile = qe->board[qe->currPlayer->yPos][qe->currPlayer->xPos + 1];
	
	if (midTile->neighbourDown == 0 || midTile->neighbourRight == 1) {
		return 0;
	}
	
	return 1;
}

void movePlayerDownRight(QE * qe) {
	qe->currPlayer->yPos = qe->currPlayer->yPos - 1;
	qe->currPlayer->xPos = qe->currPlayer->xPos + 1;
	if (qe->currPlayer->yPos == qe->currPlayer->yTarget) {
		qe->winner = qe->currPlayer->winVal;
		qe->isGameOver = 1;
	}
}

int isMovePlayerDownLeftValid(QE * qe) {

	Tile * currTile = qe->board[qe->currPlayer->yPos][qe->currPlayer->xPos];
	
	if (currTile->neighbourDown == 0) {
		return 0;
	}
	
	if (qe->currPlayer->yPos - 1 != qe->nextPlayer->yPos || qe->currPlayer->xPos != qe->nextPlayer->xPos) {
		return 0;
	}
	
	Tile * midTile = qe->board[qe->currPlayer->yPos - 1][qe->currPlayer->xPos];
	
	if (midTile->neighbourLeft == 0 || midTile->neighbourDown == 1) {
		return 0;
	}
	
	return 1;
}

int isMovePlayerLeftDownValid(QE * qe) {

	Tile * currTile = qe->board[qe->currPlayer->yPos][qe->currPlayer->xPos];
	
	if (currTile->neighbourLeft == 0) {
		return 0;
	}
	
	if (qe->currPlayer->yPos != qe->nextPlayer->yPos || qe->currPlayer->xPos - 1 != qe->nextPlayer->xPos) {
		return 0;
	}
	
	Tile * midTile = qe->board[qe->currPlayer->yPos][qe->currPlayer->xPos - 1];
	
	if (midTile->neighbourDown == 0 || midTile->neighbourLeft == 1) {
		return 0;
	}
	
	return 1;
}

void movePlayerDownLeft(QE * qe) {
	qe->currPlayer->yPos = qe->currPlayer->yPos - 1;
	qe->currPlayer->xPos = qe->currPlayer->xPos - 1;
	if (qe->currPlayer->yPos == qe->currPlayer->yTarget) {
		qe->winner = qe->currPlayer->winVal;
		qe->isGameOver = 1;
	}
}

int isMovePlayerUpLeftValid(QE * qe) {

	Tile * currTile = qe->board[qe->currPlayer->yPos][qe->currPlayer->xPos];
	
	if (currTile->neighbourUp == 0) {
		return 0;
	}
	
	if (qe->currPlayer->yPos + 1 != qe->nextPlayer->yPos || qe->currPlayer->xPos != qe->nextPlayer->xPos) {
		return 0;
	}
	
	Tile * midTile = qe->board[qe->currPlayer->yPos + 1][qe->currPlayer->xPos];
	
	if (midTile->neighbourLeft == 0 || midTile->neighbourUp == 1) {
		return 0;
	}
	
	return 1;
}

int isMovePlayerLeftUpValid(QE * qe) {

	Tile * currTile = qe->board[qe->currPlayer->yPos][qe->currPlayer->xPos];
	
	if (currTile->neighbourLeft == 0) {
		return 0;
	}
	
	if (qe->currPlayer->yPos != qe->nextPlayer->yPos || qe->currPlayer->xPos - 1 != qe->nextPlayer->xPos) {
		return 0;
	}
	
	Tile * midTile = qe->board[qe->currPlayer->yPos][qe->currPlayer->xPos - 1];
	
	if (midTile->neighbourUp == 0 || midTile->neighbourLeft == 1) {
		return 0;
	}
	
	return 1;
}

void movePlayerUpLeft(QE * qe) {
	qe->currPlayer->yPos = qe->currPlayer->yPos + 1;
	qe->currPlayer->xPos = qe->currPlayer->xPos - 1;
	if (qe->currPlayer->yPos == qe->currPlayer->yTarget) {
		qe->winner = qe->currPlayer->winVal;
		qe->isGameOver = 1;
	}
}

int isPlaceVerticalBlockValid(QE * qe, int m) {
	if (qe->currPlayer->numBlocks <= 0) {
		return 0;
	}
	
	
	if (*(qe->availBlockVertPlace + m) == 0) {
		return 0;
	}
	
	//temp place the block
	int * outPos = (int*)malloc(4*sizeof(int));
	putVertBlock(outPos, qe, m);
	int ** visitedCurr = (int**)malloc(BOARD_HEIGHT*sizeof(int*));
	int ** visitedNext = (int**)malloc(BOARD_HEIGHT*sizeof(int*));
	for (int i = 0; i < BOARD_HEIGHT; ++i) {
		*(visitedCurr + i) = (int*)calloc(BOARD_WIDTH,sizeof(int));
		*(visitedNext + i) = (int*)calloc(BOARD_WIDTH,sizeof(int));
	}
	int dfsResultCurr = dfs(qe->board[qe->currPlayer->yPos][qe->currPlayer->xPos], qe->board, qe->currPlayer->yTarget, visitedCurr, m);
	int dfsResultNext = dfs(qe->board[qe->nextPlayer->yPos][qe->nextPlayer->xPos], qe->board, qe->nextPlayer->yTarget, visitedNext, m);
	
	//WARNING not sure about this
	for (int i = 0; i < BOARD_HEIGHT; ++i) {
		free(*(visitedCurr + i));
		*(visitedCurr + i) = NULL;
		free(*(visitedNext + i));
		*(visitedNext + i) = NULL;
	}
	free(visitedCurr);
	free(visitedNext);
	visitedCurr = NULL;
	visitedNext = NULL;
	
	pickUpVertBlock(outPos, qe, m);
	free(outPos);
	outPos = NULL;
	if (dfsResultCurr == 0 || dfsResultNext == 0) {
		return 0;
	}
	
	return 1;
	
}

void placeVerticalBlock(QE * qe, int m) {
	int * outPos = (int*)malloc(4*sizeof(int));
	putVertBlock(outPos, qe, m);
	*(qe->availBlockVertPlace + m) = 0;
	qe->currPlayer->numBlocks = qe->currPlayer->numBlocks - 1;
	
	if (*(outPos + 2) < MAX_Y - 1) {
		int topBlock = posToValVertBlock(*(outPos + 2), *(outPos + 1), BOARD_WIDTH);
		*(qe->availBlockVertPlace + topBlock) = 0;
	}
	
	if (*(outPos) > MIN_Y) {
		int bottomBlock = posToValVertBlock(*(outPos) - 1, *(outPos + 1), BOARD_WIDTH);
		*(qe->availBlockVertPlace + bottomBlock) = 0;
	}
	
	int sliceBlock = posToValHorizBlock(*(outPos), *(outPos + 1), BOARD_HEIGHT);
	*(qe->availBlockHorizPlace + sliceBlock) = 0;
	
	*(qe->placedVertBlocks + m) = 1;
	
	free(outPos);
	outPos = NULL;
}

void putVertBlock(int * outPos, QE * qe, int m) {
	valToPosVertBlock(outPos, m, BOARD_WIDTH);
	
	Tile * tile0 = qe->board[*(outPos)][*(outPos + 1)];
	Tile * tile1 = qe->board[*(outPos)][*(outPos + 3)];
	tile0->neighbourRight = 0;
	tile1->neighbourLeft = 0;
	
	Tile * tile2 = qe->board[*(outPos + 2)][*(outPos + 1)];
	Tile * tile3 = qe->board[*(outPos + 2)][*(outPos + 3)];
	tile2->neighbourRight = 0;
	tile3->neighbourLeft = 0;
	
}

void pickUpVertBlock(int * outPos, QE * qe, int m) {

	Tile * tile0 = qe->board[*(outPos)][*(outPos + 1)];
	Tile * tile1 = qe->board[*(outPos)][*(outPos + 3)];
	tile0->neighbourRight = 1;
	tile1->neighbourLeft = 1;
	
	Tile * tile2 = qe->board[*(outPos + 2)][*(outPos + 1)];
	Tile * tile3 = qe->board[*(outPos + 2)][*(outPos + 3)];
	tile2->neighbourRight = 1;
	tile3->neighbourLeft = 1;
	
}

int isPlaceHorizontalBlockValid(QE * qe, int m) {
	if (qe->currPlayer->numBlocks <= 0) {
		return 0;
	}
	
	if (*(qe->availBlockHorizPlace + m) == 0) {
		return 0;
	}

	//temp place the block
	int * outPos = (int*)malloc(4*sizeof(int));
	putHorizBlock(outPos, qe, m);
	int ** visitedCurr = (int**)malloc(BOARD_HEIGHT*sizeof(int*));
	int ** visitedNext = (int**)malloc(BOARD_HEIGHT*sizeof(int*));
	for (int i = 0; i < BOARD_HEIGHT; ++i) {
		*(visitedCurr + i) = (int*)calloc(BOARD_WIDTH, sizeof(int));
		*(visitedNext + i) = (int*)calloc(BOARD_WIDTH, sizeof(int));
	}
	
	int dfsResultCurr = dfs(qe->board[qe->currPlayer->yPos][qe->currPlayer->xPos], qe->board, qe->currPlayer->yTarget, visitedCurr, m);
	int dfsResultNext = dfs(qe->board[qe->nextPlayer->yPos][qe->nextPlayer->xPos], qe->board, qe->nextPlayer->yTarget, visitedNext, m);
	//WARNING not sure about this
	for (int i = 0; i < BOARD_HEIGHT; ++i) {
		free(*(visitedCurr + i));
		*(visitedCurr + i) = NULL;
		free(*(visitedNext + i));
		*(visitedNext + i) = NULL;
	}
	free(visitedCurr);
	visitedCurr = NULL;
	free(visitedNext);
	visitedNext = NULL;
	
	pickUpHorizBlock(outPos, qe, m);
	free(outPos);
	outPos = NULL;
	if (dfsResultCurr == 0 || dfsResultNext == 0) {
		return 0;
	}
	
	return 1;
	
}

void placeHorizontalBlock(QE * qe, int m) {
	int * outPos = (int*)malloc(4*sizeof(int));
	putHorizBlock(outPos, qe, m);
	*(qe->availBlockHorizPlace + m) = 0;
	qe->currPlayer->numBlocks = qe->currPlayer->numBlocks - 1;
	
	if (*(outPos + 3) < MAX_X - 1) {
		int rightBlock = posToValHorizBlock(*(outPos), *(outPos + 3), BOARD_HEIGHT);
		*(qe->availBlockHorizPlace + rightBlock) = 0;
	}
	
	if (*(outPos + 1) > MIN_X) {
		int leftBlock = posToValHorizBlock(*(outPos), *(outPos + 1) - 1, BOARD_HEIGHT);
		*(qe->availBlockHorizPlace + leftBlock) = 0;
	}
	
	int sliceBlock = posToValVertBlock(*(outPos), *(outPos + 1), BOARD_WIDTH);
	*(qe->availBlockVertPlace + sliceBlock) = 0;
	
	*(qe->placedHorizBlocks + m) = 1;
	
	free(outPos);
	outPos = NULL;
}

void putHorizBlock(int * outPos, QE * qe, int m) {
	valToPosHorizBlock(outPos, m, BOARD_HEIGHT);
	
	Tile * tile0 = qe->board[*(outPos)][*(outPos + 1)];
	Tile * tile1 = qe->board[*(outPos + 2)][*(outPos + 1)];
	tile0->neighbourUp = 0;
	tile1->neighbourDown = 0;
	
	Tile * tile2 = qe->board[*(outPos)][*(outPos + 3)];
	Tile * tile3 = qe->board[*(outPos + 2)][*(outPos + 3)];
	tile2->neighbourUp = 0;
	tile3->neighbourDown = 0;
	
}

void pickUpHorizBlock(int * outPos, QE * qe, int m) {

	Tile * tile0 = qe->board[*(outPos)][*(outPos + 1)];
	Tile * tile1 = qe->board[*(outPos + 2)][*(outPos + 1)];
	tile0->neighbourUp = 1;
	tile1->neighbourDown = 1;
	
	Tile * tile2 = qe->board[*(outPos)][*(outPos + 3)];
	Tile * tile3 = qe->board[*(outPos + 2)][*(outPos + 3)];
	tile2->neighbourUp = 1;
	tile3->neighbourDown = 1;
	
}

int dfs(Tile * tile, Tile *** board, int yTarget, int ** visited, int action) {
	if (tile->yPos == yTarget) {
		return 1;
	}
	
	if (visited[tile->yPos][tile->xPos] == 1) {
		return 0;
	}
	
	visited[tile->yPos][tile->xPos] = 1;
	
	if (tile->neighbourUp == 1) {
		if (dfs(board[tile->yPos + 1][tile->xPos], board, yTarget, visited, action) == 1) {
			return 1;
		}
	}
	
	if (tile->neighbourDown == 1) {
		if (dfs(board[tile->yPos - 1][tile->xPos], board, yTarget, visited, action) == 1) {
			return 1;
		}
	}
	
	if (tile->neighbourRight == 1) {
		if (dfs(board[tile->yPos][tile->xPos + 1], board, yTarget, visited, action) == 1) {
			return 1;
		}
	}
	
	if (tile->neighbourLeft == 1) {
		if (dfs(board[tile->yPos][tile->xPos - 1], board, yTarget, visited, action) == 1) {
			return 1;
		}
	}
	
	return 0;
}

void updateGameState(QE * qe) {
	int * gameState = qe->gameState;
	int gameStateOffset;
	//memset will work here
	memset(qe->gameState, 0, NUM_ROWS*NUM_COLS*NUM_CHANNELS*sizeof(int));
	
	//player A pos
	*(gameState + xyzToVal(NUM_ROWS, NUM_COLS, 0, 2*qe->playerA->yPos, 2*qe->playerA->xPos)) = 1;
	
	//playerB pos
	*(gameState + xyzToVal(NUM_ROWS, NUM_COLS, 1, 2*qe->playerB->yPos, 2*qe->playerB->xPos)) = 1;
	
	//board
	for (int i = 0; i < (BOARD_HEIGHT-1)*(BOARD_WIDTH-1); ++i) {
		if (*(qe->placedVertBlocks + i) == 1) {
			int row = 2*(i / (BOARD_WIDTH-1)) + 1;
			int col = 2*(i % (BOARD_WIDTH-1)) + 1;
			
			*(gameState + xyzToVal(NUM_ROWS, NUM_COLS, 2, row-1, col)) = 1;
			*(gameState + xyzToVal(NUM_ROWS, NUM_COLS, 2, row, col)) = 1;
			*(gameState + xyzToVal(NUM_ROWS, NUM_COLS, 2, row+1, col)) = 1;
		}
		
		if (*(qe->placedHorizBlocks + i) == 1) {
			int col = 2*(i / (BOARD_HEIGHT-1)) + 1;
			int row = 2*(i % (BOARD_HEIGHT-1)) + 1;
			
			*(gameState + xyzToVal(NUM_ROWS, NUM_COLS, 2, row, col-1)) = 1;
			*(gameState + xyzToVal(NUM_ROWS, NUM_COLS, 2, row, col)) = 1;
			*(gameState + xyzToVal(NUM_ROWS, NUM_COLS, 2, row, col+1)) = 1;
		}
	}
	
	//player A blocks
	gameStateOffset = xyzToVal(NUM_ROWS, NUM_COLS, 3+qe->playerA->numBlocks, 0, 0);
	for (int i = gameStateOffset; i < gameStateOffset + NUM_ROWS*NUM_COLS; ++i) {
		*(gameState + i) = 1;
	} 
	
	//playerB blocks
	gameStateOffset = xyzToVal(NUM_ROWS, NUM_COLS, 14+qe->playerB->numBlocks, 0, 0);
	for (int i = gameStateOffset; i < gameStateOffset + NUM_ROWS*NUM_COLS; ++i) {
		*(gameState + i) = 1;
	}
	
	//has been repeated
	gameStateOffset = xyzToVal(NUM_ROWS, NUM_COLS, 24+lookup(qe->pastStates, qe->hash), 0, 0);
	for (int i = gameStateOffset; i < gameStateOffset + NUM_ROWS*NUM_COLS; ++i) {
		*(gameState + i) = 1;
	}
	
	//player
	if (qe->currPlayer == qe->playerA) {
		//memset(gameState + xyzToVal(NUM_ROWS, NUM_COLS, 28, 0, 0), 1, NUM_ROWS*NUM_COLS*sizeof(int));
		gameStateOffset = xyzToVal(NUM_ROWS, NUM_COLS, 28, 0, 0);
		for (int i = gameStateOffset; i < gameStateOffset + NUM_ROWS*NUM_COLS; ++i) {
			*(gameState + i) = 1;
		}
	}
}

char * render(QE * qe, int display) {
	char * rend = (char*)malloc(NUM_ROWS*NUM_COLS*sizeof(char));
	
	for (int row = 0; row < NUM_ROWS; row++) {
		if (row % 2 == 0) {
			renderTileRow(qe, rend, row);
		} else {
			renderBlockRow(qe, rend, row);
		}
	}
	
	if (display == 1) {
		for (int row = NUM_ROWS-1; row >= 0; row--) {
			for (int col = 0; col < NUM_COLS; col++) {
				printf(" %c ", *(rend + row*NUM_COLS + col));
			}
			printf("\n");
		}
		printf("------------------------------\n");
	}
	
	return rend;
}

void renderTileRow(QE * qe, char * rend, int row) {
	for (int col = NUM_COLS - 1; col >= 0; col--) {
		if (col % 2 == 0) {
			if (qe->playerA->yPos == row / 2 && qe->playerA->xPos == col / 2) {
				*(rend + row*NUM_COLS + col) = 'A';
			} else if (qe->playerB->yPos == row / 2 && qe->playerB->xPos == col / 2) {
				*(rend + row*NUM_COLS + col) = 'B';
			} else {
				*(rend + row*NUM_COLS + col) = '*';
			}
		} else {
			if (qe->board[row/2][col/2 + 1]->neighbourLeft == 1) {
				*(rend + row*NUM_COLS + col) = ' ';
			} else {
				*(rend + row*NUM_COLS + col) = '|';
			}
		}
	}
}

void renderBlockRow(QE * qe, char * rend, int row) {
	for (int col = NUM_COLS - 1; col >= 0; col--) {
		if (col % 2 == 0) {
			if (qe->board[row/2 + 1][col/2]->neighbourDown == 1) {
				*(rend + row*NUM_COLS + col) = ' ';
			} else {
				*(rend + row*NUM_COLS + col) = '-';
			}
		} else {
			*(rend + row*NUM_COLS + col) = ' ';
		}
	}
}




