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

void backup(Node * origNode, float v) {
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
void expandAndEvaluate(Node * node, float * p) {
	int pSum = 0;
	int * validMoves = (int*)calloc(NUM_MOVES, sizeof(int));
	
	for (int i = 0; i < NUM_MOVES; i++) {
		if (validate(node->state, i) == 1) {
			*(p + i) = expf(*(p+i));
			pSum = pSum + *(p+i);
			*(validMoves + i) = 1;
		}
	}
	//will divide not be float?
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

Node * select(Node * rootNode) {
	Node * node = rootNode;
	while (node->numChildren > 0) {
		
		//Simulate dirichlet noise.  
		//float * dirichlet = (float*)calloc(node->numChildren, sizeof(float));
		//int r = rand()%(node->numChildren);
		int diri = rand()%(node->numChildren);
		float diriVal;
		
		int index = rand() % (node->numChildren);
		
		Node * child = *(node->children + index);
		float Q = child->Q;
		float N = node->N;
		float P;
		
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
				
		float U = Cpuct * P * sqrtf(N) / (child->N + 1);
		float maxVal = Q + U;
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