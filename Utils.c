#include "Structs.h"

int posToValTile(int yPos, int xPos, int boardWidth) {
	return xPos + yPos * boardWidth;
}

void valToPosTile(int * outPos, int val, int boardWidth) {
 	*(outPos) =  val / boardWidth;
	*(outPos + 1) = val % boardWidth;
}

void valToPosVertBlock(int * outPos, int val, int boardWidth) {
	*(outPos) = val / (boardWidth - 1);
	*(outPos + 1) = val % (boardWidth - 1);
	*(outPos + 2) = *(outPos) + 1;
	*(outPos + 3) = *(outPos + 1) + 1;
}

int posToValVertBlock(int y0, int x0, int boardWidth) {
	return x0 + y0*(boardWidth - 1);
}

void valToPosHorizBlock(int * outPos, int val, int boardHeight) {
	*(outPos) = val % (boardHeight - 1);
	*(outPos + 1) = val / (boardHeight - 1);
	*(outPos + 2) = *(outPos) + 1;
	*(outPos + 3) = *(outPos + 1) + 1;
}

int posToValHorizBlock(int y0, int x0, int boardHeight) {
	return y0 + x0*(boardHeight - 1);
}

int xyzToVal(int rows, int cols, int z, int y, int x) {
	return x + y*cols + z*rows*cols;
}
