#include "Python.h"
#include "Structs.h"
#include "Quorridor.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h> 
#include <time.h>

void searchCython(int numSimulations, Tree * tree, int * gameState, float * v, 
					float * pType, float * pMove, float * pBlock, 
					int * isCReady, int * isModelReady, 
					int * numChildren, float * dirichlet, int * diriCReady, int * diriModelReady,
					int * error,
					struct timespec * tm1, struct timespec * tm2, Node ** nodes) {
	printf("Search Start \n");
	int i = 0;
	int safety = 0;
	while (i < numSimulations && safety < 1000) {
		for (int b = 0; b < BATCH_SIZE; ++b) {
			*(nodes+b) = selectMCTS(tree->rootNode, numChildren, dirichlet, diriCReady, diriModelReady,
									tm1, tm2);
			memcpy(gameState + b*NUM_CHANNELS*NUM_ROWS*NUM_COLS, (*(nodes+b))->state->gameState, NUM_ROWS*NUM_COLS*NUM_CHANNELS*sizeof(int));
		}
		*(isModelReady) = 0;
		*(isCReady) = 1;
		while (*(isModelReady) == 0) {
			//Py_BEGIN_ALLOW_THREADS MAY NEED THIS
			//sleep(1);
			nanosleep(tm1,tm2);
			//Py_END_ALLOW_THREADS MAY NEED THIS
		}
		
		//end states can be hit twice, not sure how I feel about that
		for (int b = 0; b < BATCH_SIZE; ++b) {
			if ((*(nodes+b))->numChildren == 0) {
				expandAndEvaluate(*(nodes+b), pType + b*NUM_MOVES, pMove + b*NUM_MOVES, pBlock + b*NUM_MOVES);
				++i;
				backupCython(*(nodes+b), v + b, 1);
			} else {
				*(v + b) = 0;
				backupCython(*(nodes+b), v + b, 0);
			}
			++safety;
		}
	}
	printf("Sarch End \n");
}

// void search(int numSimulations, Tree * tree) {
// 	float * p = (float*)malloc(NUM_MOVES*sizeof(float));
// 	for (int i = 0; i < numSimulations; i++) {
// 		//printf("Selecting \n");
// 		Node * node = selectMCTS(tree->rootNode);
// 		float v = ((float)rand())/((float)RAND_MAX/2) - 1;
// 		for (int i = 0; i < NUM_MOVES; i++) {
// 			*(p+i) = ((float)rand())/RAND_MAX;
// 		}
// 		//printf("Expanding \n");
// 		expandAndEvaluate(node, p);
// 		//printf("Backing \n");
// 		backup(node, v);
// 	}
// 	
// 	free(p);
// 	p = NULL;
// }

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

void playMatchCython(int numSimulations, int * gameState, float * v, float * p, 
					int * isCReady, int * isModelReady, 
					int * isCReadyForHuman, int * isHumanReady, int * humanMove,
					int * error) {

	srand(time(NULL));
	Py_BEGIN_ALLOW_THREADS
	
	struct timespec tm1,tm2;
	tm1.tv_sec = 0;                                                            
    tm1.tv_nsec = 1000;
    
	*(error) = 0;
	QE * qe = resetQE();
	Tree * tree = (Tree*)malloc(sizeof(Tree));
	tree->rootNode = createNode(qe);
	
	render(tree->rootNode->state, 1);
	while(tree->rootNode->state->isGameOver == 0) {
		//searchCython(numSimulations, tree, gameState, v, p, p, p, isCReady, isModelReady, error);
		playAgainstHuman(tree);
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
	
	Py_END_ALLOW_THREADS
}

void selfPlayCython(int numSimulations, int * gameState, float * v, 
					float * pType, float * pMove, float * pBlock, 
					int * isCReady, int * isModelReady, 
					int * numTurns, int * gameStateOut, float * vOut, float * piOut,
					float * pRChoice, int * indRChoice, int * rChoiceReadyC, int * rChoiceReadyModel,
					int * numChildren, float * dirichlet, int * diriCReady, int * diriModelReady,
					int * error) {
	srand(time(NULL));
	//May not want line directly below
	Py_BEGIN_ALLOW_THREADS
	*(error) = 0;
	QE * qe = resetQE();
	Tree * tree = (Tree*)malloc(sizeof(Tree));
	tree->rootNode = createNode(qe);

	struct timespec tm1,tm2;
	tm1.tv_sec = 0;                                                            
    tm1.tv_nsec = 1000;
    Node ** nodes = (Node**)malloc(BATCH_SIZE*sizeof(Node*));
	int stopper = 0;
	while(tree->rootNode->state->isGameOver == 0 && stopper < 500) {
		//render(tree->rootNode->state, 1);
		searchCython(numSimulations, tree, gameState, v, pType, pMove, pBlock, isCReady, isModelReady, 
					numChildren, dirichlet, diriCReady, diriModelReady, 
					error, &tm1, &tm2, nodes);
		play(tree, pRChoice, indRChoice, rChoiceReadyC, rChoiceReadyModel);
		++stopper;
	}	
	
	free(nodes);
	nodes = NULL;
	
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
		memcpy(piOut + count*NUM_MOVES, node->pi, NUM_MOVES*sizeof(float));
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