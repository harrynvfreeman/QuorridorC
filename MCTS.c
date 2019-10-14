#include "Structs.h"
#include "Quorridor.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

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
	
	return node;
}

Node * createChild(Node * parent, QE * state, int move, double p) {
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
		
	for (int i = 0; i < node->numChildren; i++) {
		Node * child = *(node->children + i);
		clearNode(child);
	}
	
	free(node->children);
	
	free(node);
}

void clearNodeSingle(Node * node) {
	//Need to free state
	clearQE(node->state);
	
	free(node->children);
	
	free(node);
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
	double pi;
	double piSum = 0;
	
	Node * node = tree->rootNode;
	
	double r = ((double)rand())/((double)(RAND_MAX));
	
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

void play(Tree * tree) {
	//int r = rand() % tree->rootNode->numChildren;
	//tree->rootNode = *(tree->rootNode->children + r);
	
	double pi;
	double piSum = 0;
	
	Node * node = tree->rootNode;
	
	double r = ((double)rand())/((double)(RAND_MAX));
	
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
	
	//Need to clear all memory for non root
	for (int i = 0; i < node->numChildren; i++) {
		if (i != index) {
			clearNode(*(node->children + i));
		}
	}
	
	tree->rootNode = nextRootNode;
}

void testPlayThisWorksNoClear(Tree * tree) {
	//int r = rand() % tree->rootNode->numChildren;
	//tree->rootNode = *(tree->rootNode->children + r);
	
	double pi;
	double piSum = 0;
	
	Node * node = tree->rootNode;
	
	double r = ((double)rand())/((double)(RAND_MAX));
	
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

void backup(Node * origNode, double v) {
	Node * node = origNode;
	while(node->hasParent == 1) {
		node->vLoss = node->vLoss - 1;
		node->W = node->W + v;
		node->Q = (node->W - node->vLoss) / (node->N);
		node = node->parent;
	}
	
	//for the parent
	node->N = node->N+1;
}

//should probability be summed before or after valid moves
void expandAndEvaluate(Node * node, double * p) {
	int pSum = 0;
	int * validMoves = (int*)calloc(NUM_MOVES, sizeof(int));
	
	for (int i = 0; i < NUM_MOVES; i++) {
		if (validate(node->state, i) == 1) {
			*(p + i) = expf(*(p+i));
			pSum = pSum + *(p+i);
			*(validMoves + i) = 1;
		}
	}
	//will divide not be double?
	for (int i = 0; i < NUM_MOVES; i++) {
		if (*(validMoves + i) == 1) {
			QE * nextState = cloneQE(node->state);
			step(nextState, i);
			createChild(node, nextState, i, *(p + i)/pSum);
		}
	}
	
	free(validMoves);
	validMoves = NULL;
}

Node * selectMCTS(Node * rootNode) {
	Node * node = rootNode;
	while (node->numChildren > 0) {
		//printf("Step0 \n");
		//Simulate dirichlet noise.  
		//double * dirichlet = (double*)calloc(node->numChildren, sizeof(double));
		//int r = rand()%(node->numChildren);
		int diri = rand()%(node->numChildren);
		double diriVal;
		
		int index = rand() % (node->numChildren);
		Node * child = *(node->children + index);
		//printf("%d \n", node->numChildren);
		double Q = child->Q;
		double N = node->N;
		double P;
		
		if (node->hasParent == 1) {
			P = child->P;
		} else {
			//*(dirichlet + r) = 1;
			//P = (1-e)*(child->P) + e*(*(dirichlet + index));
			if (diri == index) {
				diriVal = 0.9;
			} else {
				diriVal = 0.1;
			}
			//P = (1-e)*(child->P) + e*(*(dirichlet + index));
			P = (1-e)*(child->P) + e*diriVal;
		}
		double U = Cpuct * P * sqrtf(N) / (child->N + 1);
		double maxVal = Q + U;
		Node * maxChild = child;
		int maxInd = index;
		for (int i = 1; i < node->numChildren; i++) {
			index = (index + 1) % node->numChildren;
			
			if (diri == index) {
				diriVal = 0.9;
			} else {
				diriVal = 0.1;
			}
			
			child = *(node->children + index);
			
			Q = child->Q;
			
			if (node->hasParent == 1) {
				P = child->P;
			} else {
				//P = (1-e)*(child->P) + e*(*(dirichlet + index));
				P = (1-e)*(child->P) + e*diriVal;
			}
			
			U = Cpuct * P * sqrtf(N) / (child->N + 1);
			if (Q + U > maxVal) {
				maxVal = Q + U;
				maxChild = child;
				maxInd = i;
			}
			
		}
		maxChild->vLoss = maxChild->vLoss + 1;
		maxChild->N = maxChild->N + 1;
		maxChild->Q = (maxChild->W - maxChild->vLoss)/(maxChild->N);
		node = maxChild;
		
		//free(dirichlet);
		//dirichlet = NULL;
	}
	
	return node;
}