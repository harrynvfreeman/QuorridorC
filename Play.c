#include "Python.h"
#include "Structs.h"
#include "Quorridor.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h> 
#include <time.h>

void searchCython(int numSimulations, Tree * tree, int * gameState, double * v, 
					double * p, int * isCReady, int * isModelReady, int * error) {
	struct timespec tm1,tm2;
	tm1.tv_sec = 0;                                                            
    tm1.tv_nsec = 1000;
	Node ** nodes = (Node**)malloc(BATCH_SIZE*sizeof(Node*));
	double * vCopy = (double*)malloc(sizeof(double));
	double * pCopy = (double*)malloc(NUM_MOVES*sizeof(double));
	int i = 0;
	int safety = 0;
	while (i < numSimulations && safety < 1000) {
		for (int b = 0; b < BATCH_SIZE; ++b) {
			*(nodes+b) = selectMCTS(tree->rootNode);
			memcpy(gameState + b*NUM_CHANNELS*NUM_ROWS*NUM_COLS, (*(nodes+b))->state->gameState, NUM_ROWS*NUM_COLS*NUM_CHANNELS*sizeof(int));
		}
		*(isModelReady) = 0;
		*(isCReady) = 1;
		while (*(isModelReady) == 0) {
			//Py_BEGIN_ALLOW_THREADS MAY NEED THIS
			//sleep(1);
			nanosleep(&tm1,&tm2);
			//Py_END_ALLOW_THREADS MAY NEED THIS
		}
		
		//end states can be hit twice, not sure how I feel about that
		for (int b = 0; b < BATCH_SIZE; ++b) {
			if ((*(nodes+b))->numChildren == 0) {
				memcpy(vCopy, v + b, sizeof(double));
				memcpy(pCopy, p + b*NUM_MOVES, NUM_MOVES*sizeof(double));
				expandAndEvaluate(*(nodes+b), pCopy);
				++i;
			} else {
				*vCopy = 0;
			}
			backupCython(*(nodes+b), vCopy);
			++safety;
		}
	}
	free(nodes);
	nodes = NULL;
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
		//Py_BEGIN_ALLOW_THREADS;
		nanosleep(&tm1,&tm2);
		//sleep(1);
		//Py_END_ALLOW_THREADS
	}
	
	printf("Wait is %d for thread %d \n", *(wait), threadNum);
}

void playMatchCython(int numSimulations, int * gameState, double * v, double * p, 
					int * isCReady, int * isModelReady, 
					int * isCReadyForHuman, int * isHumanReady, int * humanMove,
					double * pRChoice, int * indRChoice, int * rChoiceReadyC, int * rChoiceReadyModel,
					int * error) {

	srand(time(NULL));
	//Py_BEGIN_ALLOW_THREADS
	
	struct timespec tm1,tm2;
	tm1.tv_sec = 0;                                                            
    tm1.tv_nsec = 1000;
    
	*(error) = 0;
	QE * qe = resetQE();
	Tree * tree = (Tree*)malloc(sizeof(Tree));
	tree->rootNode = createNode(qe);
	
	render(tree->rootNode->state, 1);
	while(tree->rootNode->state->isGameOver == 0) {
		searchCython(numSimulations, tree, gameState, v, p, isCReady, isModelReady, error);
		play(tree, pRChoice, indRChoice, rChoiceReadyC, rChoiceReadyModel);
		render(tree->rootNode->state, 1);
		if (tree->rootNode->state->isGameOver == 0) {
			*(isHumanReady) = 0;
			*(isCReadyForHuman) = 1;
			while (*(isHumanReady) == 0) {
				nanosleep(&tm1,&tm2);
			}
			playHuman(tree, *humanMove);
			render(tree->rootNode->state, 1);
		}
	}
	
	//Not freeing right now, will have to
	printf("Winner is: %d \n", tree->rootNode->state->winner);
	
	//Py_END_ALLOW_THREADS
}

void selfPlayCython(int numSimulations, int * gameState, double * v, double * p, 
					int * isCReady, int * isModelReady, 
					int * numTurns, int * gameStateOut, double * vOut, double * piOut,
					double * pRChoice, int * indRChoice, int * rChoiceReadyC, int * rChoiceReadyModel,
					int * error) {
	srand(time(NULL));
	//May not want line directly below
	Py_BEGIN_ALLOW_THREADS
	*(error) = 0;
	QE * qe = resetQE();
	Tree * tree = (Tree*)malloc(sizeof(Tree));
	tree->rootNode = createNode(qe);

	int stopper = 0;
	while(tree->rootNode->state->isGameOver == 0 && stopper < 500) {
		//render(tree->rootNode->state, 1);
		searchCython(numSimulations, tree, gameState, v, p, isCReady, isModelReady, error);
		play(tree, pRChoice, indRChoice, rChoiceReadyC, rChoiceReadyModel);
		++stopper;
	}	
	
	//render(tree->rootNode->state, 1);
	if (stopper >= 500) {
		printf("SOMETHING WENT WRONG UH OH");
		*(error) = 1;
	}
	
	*numTurns = tree->rootNode->state->turnNum;
	
	int count = tree->rootNode->state->turnNum - 1;
	int winner = tree->rootNode->state->winner;
	Node * node = tree->rootNode;
	Node * temp;
	while(node->parent != NULL) {
		temp = node->parent;
		clearNodeSingle(node);
		node = temp;
		
		memcpy(gameStateOut + count*NUM_CHANNELS*NUM_ROWS*NUM_COLS, node->state->gameState, 
				NUM_CHANNELS*NUM_ROWS*NUM_COLS*sizeof(int));
		int vVal;
		if (winner == 0) {
			vVal = 0;
		} else if (winner == node->state->currPlayer->winVal) {
			vVal = 1;
		} else {
			vVal = -1;
		}
		*(vOut + count) = vVal;
		memcpy(piOut + count*NUM_MOVES, node->pi, NUM_MOVES*sizeof(double));
		--count;
	}
	
	clearNodeSingle(node);
	free(tree);
	tree = NULL;
	
	Py_END_ALLOW_THREADS
	//May not want line directly above
}


// void selfPlay(int numSimulations, Tree * tree) {
// 	int stopper = 0;
// 	while(tree->rootNode->state->isGameOver == 0 && stopper < 500) {
// 		render(tree->rootNode->state, 1);
// 		search(numSimulations, tree);
// 		play(tree);
// 		stopper = stopper + 1;
// 	}	
// 	
// 	if (stopper >= 500) {
// 		printf("SOMETHING WENT WRONG GAME KEPT GOING");
// 	}
// 	
// 	Node * node = tree->rootNode;
// 	Node * temp;
// 	while(node->parent != NULL) {
// 		temp = node->parent;
// 		clearNodeSingle(node);
// 		node = temp;
// 	}
// 	clearNodeSingle(node);
// 	free(tree);
// 	tree = NULL;
// 	
// }