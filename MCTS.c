#include "Structs.h"
#include "Quorridor.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

Node * createNode(QE * state) {
	Node * node = (Node*)malloc(sizeof(Node));
	node->state = state;
	node->children = (Node**)malloc(NUM_MOVES*sizeof(Node*));
	node->numChildren = 0;
	node->parent = NULL;
	node->hasParent = 0;
	node->N = 0;
	node->W = 0;
	node->Q = 0;
	node->P = 0;
	node->vLoss = 0;
	node->move = -1;
	node->pi = (float*)calloc(NUM_MOVES, sizeof(float));
	
	return node;
}

Node * createChild(Node * parent, QE * state, int move, float p) {
	Node * child = createNode(state);
	child->parent = parent;
	child->hasParent = 1;
	child->move = move;
	child->P = p;
	*(parent->children + parent->numChildren) = child;
	parent->numChildren = parent->numChildren + 1; 
	
	return child;
}

void clearNode(Node * node) {
	//Need to free state
	clearQE(node->state);
		
	for (int i = 0; i < node->numChildren; ++i) {
		Node * child = *(node->children + i);
		clearNode(child);
	}
	
	free(node->pi);
	node->pi = NULL;
	
	free(node->children);
	node->children = NULL;
	
	node->parent = NULL;
	free(node);
	node = NULL;
}

void clearNodeSingle(Node * node) {
	//Need to free state
	clearQE(node->state);
	
	free(node->pi);
	node->pi = NULL;
	
	free(node->children);
	node->children = NULL;
	
	node->parent = NULL;
	free(node);
	node = NULL;
}



// void clearSingleNode(Node * node) {
// 	//Need to free state
// 	clearQE(node->state);
// 	free(node->parent);
// 	free(node->N);
// 	free(node->Q);
// 	free(node->W);
// 	free(node->P);
// 	free(node->vLoss);
// 	free(node->children);
// 	
// 	free(node);
// }

//Ignoring temperature for now
//Ignoring resignation for now
//can remove parent removal if we want to debug
//actually I am going to keep the parent so I can keep the history
void playButDoesNotWorkIdkWhy(Tree * tree) {	
	float pi;
	float piSum = 0;
	
	Node * node = tree->rootNode;
	
	float r = ((float)rand())/((float)(RAND_MAX));
	
	int index = 0;
	int move = -1;
	
	while (index < node->numChildren && move == -1) {
		pi = (*(node->children + index))->N / node->N;
		piSum = piSum + pi;
		if (r <= piSum) {
			//move = (*(node->children + index))->move;
			move = 0;
		}
		
		index = index + 1;
	}
	
	//For debugging
	if (move == -1) {
		// printf("Something went horribly horribly wrong \n");
// 		printf("piSum is %f \n", piSum);
// 		for (int debug = 0; debug < node->numChildren; debug++) {
// 			pi = (*(node->children + debug))->N / node->N;
// 			printf("pi is %f for move %d \n", pi, (*(node->children + debug))->move);
// 		}
		index = index -1;
		//move = (*(node->children + index))->move;
	}
	
	Node * nextNode = *(node->children + index);
	//nextNode->parent = NULL;  For now keep parent, but set has parent to 0
	nextNode->hasParent = 0;
	
	//Need to clear all memory for non root
	// for (int i = 0; i < node->numChildren; i++) {
// 		if (i != index) {
// 			clearNode(*(node->children + i));
// 		}
// 	}
	
	//clearSingleNode(node); Will not clear.  Keeping parent history, except for children
	tree->rootNode = nextNode;
	
}

void playHuman(Tree * tree, int move) {
	//Start with just resetting the tree
	//Later we can not reset it
	printf("Move is: %d \n", move);
	Node * node = tree->rootNode;
	
	QE * nextState = cloneQE(node->state);
	step(nextState, move);
	
	tree->rootNode = createNode(nextState);
	
	clearNode(node);
}

void playAgainstHuman(Tree * tree) {
	Node * node = tree->rootNode;
	float maxVal = 0;
	int maxInd = 0;
	for (int i = 0; i < node->numChildren; ++i) {
		Node * child = *(node->children + i);
		if (child->N > maxVal) {
			maxVal = child->N;
			maxInd = i;
		}
	}
	
	Node * nextRootNode = *(node->children + maxInd);
	nextRootNode->hasParent = 0;
	
	for (int i = 0; i < node->numChildren; ++i) {
		if (i != maxInd) {
			clearNode(*(node->children + i));
		}
	}

	tree->rootNode = nextRootNode;
	
}

void play(Tree * tree, float * pRChoice, int * indRChoice, int * rChoiceReadyC, int * rChoiceReadyModel) {
	//int r = rand() % tree->rootNode->numChildren;
	//tree->rootNode = *(tree->rootNode->children + r);
	
	float pi;
	//float piSum = 0;
	
	Node * node = tree->rootNode;
	
	//float r = ((float)rand())/((float)(RAND_MAX));
	//printf("r is: %f \n", r);
	
	float N = 0;
	for (int i = 0; i < node->numChildren; ++i) {
		N = N + (*(node->children + i))->N;
	}
	
	// for (int i = 0; i < NUM_CHANNELS; i++) {
// 		for (int j = 0; j < NUM_ROWS; j++) {
// 			for (int k = 0; k < NUM_COLS; k++) {
// 				printf("%d, ", *(node->state->gameState + i*NUM_ROWS*NUM_COLS + j*NUM_COLS + k));
// 			}
// 			printf("\n");
// 		}
// 		printf("\n");
// 		printf("\n");
// 		printf("\n");
// 		printf("\n");
// 		printf("\n");
// 	}
	
	//int finalIndex = -1;	
 	// printf("Node N is: %f \n", node->N);
//  	printf("Node children is: %d \n", node->numChildren);
//  	printf("Child N's are: ");
 	memset(pRChoice, 0, NUM_MOVES*sizeof(float));
	for (int index = 0; index < node->numChildren; ++index) {
		Node * child = *(node->children + index);
		// printf("%f, ", child->N);
		pi = child->N / N;
		*(node->pi + child->move) = pi;
		*(pRChoice + index) = pi;
		//piSum = piSum + pi; //moved out here for debugging, can move back in
		//printf("PSum is now: %f \n", piSum);
		//if (finalIndex == -1) {
			//piSum = piSum + pi;
			//if (r <= piSum) {
			//	finalIndex = index;
			//}
		//}
	}
 	// printf("\n");
//  	printf("PiSum is: %f \n", piSum);
//  	printf("Pi is: ");
//  	for (int i = 0; i < NUM_MOVES; i++) {
//  	printf("(%d, %f, %d), ", i, *(node->pi + i), validate(node->state, i));
//  	}
//  	printf("\n");
	
	
	
	// if (finalIndex == -1) {
// 		finalIndex = node->numChildren - 1;
// 	}
	
	struct timespec tm1,tm2;
	tm1.tv_sec = 0;                                                            
    tm1.tv_nsec = 1000;
    *(rChoiceReadyModel) = 0;
	*(rChoiceReadyC) = 1;
	while (*(rChoiceReadyModel) == 0) {
		nanosleep(&tm1,&tm2);
	}
	
	//printf("random move selected was: %d \n", *indRChoice);
	
	//Node * nextRootNode = *(node->children + finalIndex);
	Node * nextRootNode = *(node->children + *indRChoice);
	//printf("Move is: %d \n", nextRootNode->move);
	nextRootNode->hasParent = 0;
	
	//Need to clear all memory for non root
	for (int i = 0; i < node->numChildren; ++i) {
		//if (i != finalIndex) {
		if (i != *indRChoice) {
			clearNode(*(node->children + i));
		}
	}
	//printf("Did we make it to here \n");
	tree->rootNode = nextRootNode;
}

void testPlayThisWorksNoClear(Tree * tree) {
	//int r = rand() % tree->rootNode->numChildren;
	//tree->rootNode = *(tree->rootNode->children + r);
	
	float pi;
	float piSum = 0;
	
	Node * node = tree->rootNode;
	
	float r = ((float)rand())/((float)(RAND_MAX));
	
	int index = 0;
	int found = 0;
	
	while (index < node->numChildren - 1 && found == 0) {
		pi = (*(node->children + index))->N / node->N;
		piSum = piSum + pi;
		if (r <= piSum) {
			found = 1;
		}
		index = index + 1;
	}
	
	Node * nextRootNode = *(node->children + index);
	nextRootNode->hasParent = 0;
	
	tree->rootNode = nextRootNode;
}

void backup(Node * origNode, float v) {
	Node * node = origNode;
	while(node->hasParent == 1) {
		node->vLoss = node->vLoss - 1;
		node->W = node->W + v;
		node->N = node->N + 1; //if remove then uncomment in search
		node->Q = (node->W - node->vLoss) / (node->N);
		node = node->parent;
	}
	
	//for the root
	// if (origNode->hasParent == 0) {
// 		node->N = node->N + 1;
// 	}
}

void backupCython(Node * origNode, float * v, int shouldUpdateN) {
	Node * node = origNode;
	while(node->hasParent == 1) {
		node->vLoss = node->vLoss - 1;
		node->W = node->W + *v;
		node->N = node->N + shouldUpdateN; //if remove then uncomment in search
		node->Q = (node->W - node->vLoss) / (node->N);
		node = node->parent;
	}
	
	//for the root
	// if (origNode->hasParent == 0) {
// 		node->N = node->N + 1;
// 	}
}

//should probability be summed before or after valid moves
void expandAndEvaluate(Node * node, float * pType, float * pMove, float * pBlock) {
	float pSum = 0;
	int * validMoves = (int*)calloc(NUM_MOVES, sizeof(int));
	float * p = (float*)malloc(NUM_MOVES*sizeof(float));
	*(pType) = expf(*(pType));
	*(pType + 1) = expf(*(pType + 1));
	
	for (int i = 0; i < 12; ++i) {
		if (validate(node->state, i) == 1) {
			*(p + i) = expf(*(pMove+i)) * (*(pType));
			pSum = pSum + *(p+i);
			*(validMoves + i) = 1;
		} 
	}
	
	for (int i = 12; i < NUM_MOVES; ++i) {
		if (validate(node->state, i) == 1) {
			*(p + i) = expf(*(pBlock+i-12)) * (*(pType + 1));
			pSum = pSum + *(p+i);
			*(validMoves + i) = 1;
		}
	
	}
	
	for (int i = 0; i < NUM_MOVES; ++i) {
		if (*(validMoves + i) == 1) {
			QE * nextState = cloneQE(node->state);
			step(nextState, i);
			createChild(node, nextState, i, *(p + i)/pSum);
		}
	}
	
	free(p);
	free(validMoves);
	p = NULL;
	validMoves = NULL;
}

Node * selectMCTS(Node * rootNode, int * numChildren, float * dirichlet, 
				int * diriCReady, int * diriModelReady) {
	Node * node = rootNode;
	struct timespec tm1,tm2;
	tm1.tv_sec = 0;                                                            
    tm1.tv_nsec = 1000;
	while (node->numChildren > 0) {
		
		int index = rand() % (node->numChildren);
		Node * child = *(node->children + index);
		//printf("%d \n", node->numChildren);
		float Q = child->Q;
		float N;
		float P;
		
		if (node->hasParent == 1) {
			P = child->P;
			N = node->N;
		} else {
			*numChildren = node->numChildren;
			*(diriModelReady) = 0;
			*(diriCReady) = 1;
			while (*(diriModelReady) == 0) {
				nanosleep(&tm1,&tm2);
			}
			P = (1-e)*(child->P) + e*(*(dirichlet + index));
			
			N = 0;
			for (int i = 0; i < node->numChildren; i++) {
				N = N + (*(node->children + i))->N;
			}
			
		}
		float U = Cpuct * P * sqrtf(N) / (child->N + 1);
		float maxVal = Q + U;
		Node * maxChild = child;
		for (int i = 1; i < node->numChildren; ++i) {
			index = (index + 1) % node->numChildren;
			
			child = *(node->children + index);
			
			Q = child->Q;
			
			if (node->hasParent == 1) {
				P = child->P;
			} else {
				P = (1-e)*(child->P) + e*(*(dirichlet + index));
			}
			
			U = Cpuct * P * sqrtf(N) / (child->N + 1);
			if (Q + U > maxVal) {
				maxVal = Q + U;
				maxChild = child;
			}
			
		}
		maxChild->vLoss = maxChild->vLoss + 1;
		//maxChild->N = maxChild->N + 1; if re adding remove update in backup
		maxChild->Q = (maxChild->W - maxChild->vLoss)/(maxChild->N);
		node = maxChild;
		
		//free(dirichlet);
		//dirichlet = NULL;
	}
	return node;
}