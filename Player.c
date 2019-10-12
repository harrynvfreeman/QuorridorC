#include "Structs.h"
#include "Quorridor.h"
#include <stdlib.h>

Player * initPlayer(int yPos, int xPos, int yTarget, int numBlocks, int winVal) {
	Player * player = (Player*)malloc(sizeof(Player));
	player->yPos = yPos;
	player->xPos = xPos;
	player->yTarget = yTarget;
	player->numBlocks = numBlocks;
	player->winVal = winVal;
	
	return player;
}

Player * clonePlayer(Player * cloneFrom) {
	Player * player = (Player*)malloc(sizeof(Player));
	player->yPos = cloneFrom->yPos;
	player->xPos = cloneFrom->xPos;
	player->yTarget = cloneFrom->yTarget;
	player->numBlocks = cloneFrom->numBlocks;
	player->winVal = cloneFrom->winVal;
	
	return player;
}