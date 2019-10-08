#include "Structs.h"
#include "Quorridor.h"
#include <stdlib.h>

Tile * initTile(int yPos, int xPos, int minY, int minX, int maxY, int maxX) {
	Tile * tile = (Tile*)malloc(sizeof(Tile));
	tile->yPos = yPos;
	tile->xPos = xPos;
	
	if (yPos < minY) {
		tile->neighbourUp = 0;
		tile->neighbourDown = 0;
	} else if (yPos == minY) {
		tile->neighbourUp = 1;
		tile->neighbourDown = 0;
	} else if (yPos > maxY - 1) {
		tile->neighbourUp = 0;
		tile->neighbourDown = 0;
	} else if (yPos == maxY - 1) {
		tile->neighbourUp = 0;
		tile->neighbourDown = 1;
	} else {
		tile->neighbourUp = 1;
		tile->neighbourDown = 1;
	}
	
	if (xPos < minX) {
		tile->neighbourRight = 0;
		tile->neighbourLeft = 0;
	} else if (xPos == minX) {
		tile->neighbourRight = 1;
		tile->neighbourLeft = 0;
	} else if (xPos > maxX - 1) {
		tile->neighbourRight = 0;
		tile->neighbourLeft = 0;
	} else if (xPos == maxX - 1) {
		tile->neighbourRight = 0;
		tile->neighbourLeft = 1;
	} else {
		tile->neighbourRight = 1;
		tile->neighbourLeft = 1;
	}
	
	return tile;
}