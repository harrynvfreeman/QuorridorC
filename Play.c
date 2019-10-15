#include "Python.h"
#include "Structs.h"
#include "Quorridor.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h> 

//v is size 1
//p is size numThreads*NUM_MOVES*1
//assumes isCReady is 1
void searchCython(int numSimulations, Tree * tree, int * gameState, double * v, 
					double * p, int * isCReady, int * isModelReady, int * error) {
	*(isModelReady) = 0;
	//printf("got here \n");
	for (int i = 0; i < numSimulations; i++) {
		//printf("selecting \n");
		Node * node = selectMCTS(tree->rootNode);
		
		memcpy(gameState, node->state->gameState, NUM_ROWS*NUM_COLS*NUM_CHANNELS*sizeof(int));
		*(isCReady) = 1;
		//printf("here C \n");
		//Py_BEGIN_ALLOW_THREADS
		while (*(isModelReady) == 0) {
			//Py_BEGIN_ALLOW_THREADS
			sleep(1);
			//Py_END_ALLOW_THREADS
		}
		//Py_END_ALLOW_THREADS
		//printf("Dunzo C \n");
		expandAndEvaluate(node, p);
		backup(node, *v);
	}
}

void search(int numSimulations, Tree * tree) {
	double * p = (double*)malloc(NUM_MOVES*sizeof(double));
	for (int i = 0; i < numSimulations; i++) {
		//printf("Selecting \n");
		Node * node = selectMCTS(tree->rootNode);
		double v = ((double)rand())/((double)RAND_MAX/2) - 1;
		for (int i = 0; i < NUM_MOVES; i++) {
			*(p+i) = ((double)rand())/RAND_MAX;
		}
		//printf("Expanding \n");
		expandAndEvaluate(node, p);
		//printf("Backing \n");
		backup(node, v);
	}
	
	free(p);
	p = NULL;
}

void selfPlayCython(int numSimulations, int * gameState, double * v, double * p, 
					int * isCReady, int * isModelReady, int * error) {
	Py_BEGIN_ALLOW_THREADS
	//printf("Starting \n");
	*(error) = 0;
	QE * qe = resetQE();
	Tree * tree = (Tree*)malloc(sizeof(Tree));
	tree->rootNode = createNode(qe);
	//printf("node comp \n");
	int stopper = 0;
	while(tree->rootNode->state->isGameOver == 0 && stopper < 500) {
		render(tree->rootNode->state, 1);
		//printf("searching \n");
		searchCython(numSimulations, tree, gameState, v, p, isCReady, isModelReady, error);
		play(tree);
		stopper = stopper + 1;
	}	
	render(tree->rootNode->state, 1);
	//printf('Winner is: %d \n' + tree->rootNode->state->winner);
	if (stopper >= 500) {
		printf("SOMETHING WENT WRONG GAME KEPT GOING");
		*(error) = 1;
	}
	
	// Node * node = tree->rootNode;
// 	printf("a \n");
// 	Node * temp;
// 	while(node->parent != NULL) {
// 		printf("olay \n");
// 		temp = node->parent;
// 		clearNodeSingle(node);
// 		node = temp;
// 	}
// 	printf("b \n");
// 	clearNodeSingle(node);
// 	printf("c \n");
// 	free(tree);
// 	printf("d \n");
	printf("Endo \n");
	Py_END_ALLOW_THREADS
}


void selfPlay(int numSimulations, Tree * tree) {
	int stopper = 0;
	while(tree->rootNode->state->isGameOver == 0 && stopper < 500) {
		render(tree->rootNode->state, 1);
		search(numSimulations, tree);
		play(tree);
		stopper = stopper + 1;
	}	
	
	if (stopper >= 500) {
		printf("SOMETHING WENT WRONG GAME KEPT GOING");
	}
	
}

// void play(Tree * tree) {	
// 	double * pi = (double *)calloc(NUM_MOVES, sizeof(double));
// 	
// 	Node * node = tree->rootNode;
// 	
// 	for (int i = 0; i < node->numChildren; i++) {
// 		*(pi + (*(node->children + i))->move) = (*(node->children + i))->N / node->N;
// 	}
// }


// int choices(double * p) {
// 	double r = ((double)rand())/RAND_MAX;
// 	
// 	double sum = 0;
// 	
// 	for (int i = 0; i < NUM_MOVES; i++) {
// 		sum = sum + *(p+i);
// 		if (r < sum) {
// 			return i;
// 		}
// 	}
// 	
// 	printf("Something went horribly horribly wrong here");
// 	
// 	return NUM_MOVES - 1;
// }