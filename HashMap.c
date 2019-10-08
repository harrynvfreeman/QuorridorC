#include "Structs.h"
#include "Quorridor.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int compareHash(int * a, int * b) {
	for (int i = 0; i < hash_size; i++) {
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
    nodeToAdd->key = key;
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
	MapNode * head = map->list;
	MapNode * temp = head;
	while(temp) {
		head = temp->next;
		//WARNING about free hash
		free(temp->key);
		free(temp);
		temp = head;
	}
	
	map->list = NULL;
}