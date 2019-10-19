#include "Python.h"
#include "Structs.h"
#include "Quorridor.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h> 
#include <time.h>

//v is size 1
//p is size numThreads*NUM_MOVES*1
//assumes isCReady is 1
void searchCython(int numSimulations, Tree * tree, int * gameState, double * v, 
					double * p, int * isCReady, int * isModelReady, int * error) {
	struct timespec tm1,tm2;
	tm1.tv_sec = 0;                                                            
    tm1.tv_nsec = 1000;
	Node * node;
	double * vCopy = (double*)malloc(sizeof(double));
	double * pCopy = (double*)malloc(NUM_MOVES*sizeof(double));
	for (int i = 0; i < numSimulations; i++) {
		node = selectMCTS(tree->rootNode);
		//printf("I could selectd %d \n", i);
		//printf("model is: %d \n", isModelReady);
		//fflush(stdout);
		memcpy(gameState, node->state->gameState, NUM_ROWS*NUM_COLS*NUM_CHANNELS*sizeof(int));
		*(isModelReady) = 0;
		*(isCReady) = 1;
		while (*(isModelReady) == 0) {
			//Py_BEGIN_ALLOW_THREADS MAY NEED THIS
			//sleep(1);
			nanosleep(&tm1,&tm2);
			//Py_END_ALLOW_THREADS MAY NEED THIS
		}
		memcpy(vCopy, v, sizeof(double));
		memcpy(pCopy, p, NUM_MOVES*sizeof(double));
		expandAndEvaluate(node, pCopy);
		backupCython(node, vCopy);
	}
	//printf("Done cython searching \n");
	free(vCopy);
	vCopy = NULL;
	free(pCopy);
	pCopy = NULL;
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

void cFunctionWorking(int threadNum, int * val, int * wait) {	
	struct timespec tm1,tm2;
	tm1.tv_sec = 0;                                                            
    tm1.tv_nsec = 1000000; 
	while (*(wait) == 0) {
		*val = *(val) + 1;
		//printf("Val is now: %d for thread %d \n", *(val), threadNum);
		Py_BEGIN_ALLOW_THREADS;
		nanosleep(&tm1,&tm2);
		//sleep(1);
		Py_END_ALLOW_THREADS
	}
	
	printf("Wait is %d for thread %d \n", *(wait), threadNum);
}

void selfPlayCython(int numSimulations, int * gameState, double * v, double * p, 
					int * isCReady, int * isModelReady, int * error) {
	//May not want line directly below
	Py_BEGIN_ALLOW_THREADS
	*(error) = 0;
	QE * qe = resetQE();
	Tree * tree = (Tree*)malloc(sizeof(Tree));
	tree->rootNode = createNode(qe);

	int stopper = 0;
	while(tree->rootNode->state->isGameOver == 0 && stopper < 500) {
// 		printf("Num sims is: %d \n", numSimulations);
// 		printf("gameState is is: %d, %d, %d \n", *(gameState), *(gameState + 1), *(gameState + 2));
// 		printf("v is: %f \n", *(v));
// 		printf("p is: %f, %f, %f \n", *(p), *(p+1), *(p+2));
// 		printf("cReady is: %d \n", *(isCReady));
// 		printf("Model Ready is: %d \n", *(isModelReady));
		//printf("Rend \n");
		render(tree->rootNode->state, 1);
		searchCython(numSimulations, tree, gameState, v, p, isCReady, isModelReady, error);
		play(tree);
		stopper = stopper + 1;
	}	
	printf("Bend \n");
	render(tree->rootNode->state, 1);
	if (stopper >= 500) {
		printf("SOMETHING WENT WRONG GAME KEPT GOING");
		*(error) = 1;
	}
	
	Node * node = tree->rootNode;
	Node * temp;
	while(node->parent != NULL) {
		temp = node->parent;
		clearNodeSingle(node);
		node = temp;
	}
	printf("Got here \n");
	clearNodeSingle(node);
	free(tree);
	tree = NULL;
	
	printf("Endo \n");
	Py_END_ALLOW_THREADS
	//May not want line directly above
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
	
	Node * node = tree->rootNode;
	Node * temp;
	while(node->parent != NULL) {
		temp = node->parent;
		clearNodeSingle(node);
		node = temp;
	}
	clearNodeSingle(node);
	free(tree);
	tree = NULL;
	
}