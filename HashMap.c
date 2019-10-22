#include "Structs.h"
#include "Quorridor.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

MapNode * cloneMapNode(MapNode * cloneFrom);

//there is a function to make this quicker.
int compareHash(int * a, int * b) {
	for (int i = 0; i < hash_size; ++i) {
		if (*(a+i) != *(b+i)) {
			return -1;
		}
	}
	
	return 1;
}

HashMap * createHashMap() {
	HashMap * map = (HashMap*)malloc(sizeof(HashMap));
	
	map->list = NULL;
	
	return map;
}

HashMap * cloneMap(HashMap * cloneFrom) {
	HashMap * map = (HashMap*)malloc(sizeof(HashMap));
	
	if (cloneFrom->list == NULL) {
		map->list = NULL;
		return map;
	}
	
	MapNode * cloneFromNode = cloneFrom->list;
	MapNode * curr = cloneMapNode(cloneFromNode);
	map->list = curr;
	
	while(cloneFromNode->next != NULL) {
		cloneFromNode = cloneFromNode->next;
		curr->next = cloneMapNode(cloneFromNode);
		curr = curr->next;
	}
	
	return map;
}

MapNode * cloneMapNode(MapNode * cloneFrom) {
	MapNode * mapNode = (MapNode*)malloc(sizeof(MapNode));
	int * key = (int*)malloc(hash_size*sizeof(int));
	memcpy(key, cloneFrom->key, hash_size*sizeof(int));
	int val = cloneFrom->val;
	
	mapNode->key = key;
	mapNode->val = val;
	mapNode->next = NULL;
	
	return mapNode;
}

// int hashCode(HashMap * map,int key){
//     return key % map->size;
// }

void insert(HashMap * map, int * key, int val) {
    //int hash = hashCode(map, key);
    MapNode * head = map->list;
    MapNode * temp = head;
    while(temp){
        //if(memcmp(temp->key, key, hash_size*sizeof(int)) == 0){
        if (compareHash(temp->key, key) == 1) {
            temp->val = val;
            return;
        }
        temp = temp->next;
    }
    
    MapNode * nodeToAdd = (MapNode*)malloc(sizeof(MapNode));
    int * keyCopy = (int *)malloc(hash_size*sizeof(int));
    memcpy(keyCopy, key, hash_size*sizeof(int));
    nodeToAdd->key = keyCopy;
    nodeToAdd->val = val;
    nodeToAdd->next = head;
    map->list = nodeToAdd;
}

int lookup(HashMap * map, int * key) {
    //int hash = hashCode(map, key);
    MapNode * head = map->list;
    MapNode * temp = head;
    while(temp){
    	if (compareHash(temp->key, key) == 1) {
        //if(memcmp(temp->key, key, hash_size*sizeof(int)) == 0){
            return temp->val;
        }
        temp = temp->next;
    }
    return -1;
}

void empty(HashMap * map) {
	int i = 0;
	MapNode * curr = map->list;
	MapNode * next;
	while(curr != NULL) {
		next = curr->next;
		//WARNING about free hash
		free(curr->key);
		curr->key = NULL;
		free(curr);
		curr = next;
		++i;
	}
	
	map->list = NULL;
}