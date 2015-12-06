#include "hashmap.h"
#include <stddef.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

// Declares the strdup function in string.h, because c99 does not normally support it.
char* strdup(const char * s);

typedef unsigned int Hash; 

typedef Hash (*HashFunction)(char*); 

typedef struct Node {
	struct Node *next;
	char *key;
	void *value;
	} Node;

typedef struct _HashMap {
	size_t key_space;
	struct Node **buckets;
	HashFunction hash;
	} HashMap;

Hash default_hash(char *key) {
	Hash value = 0;
	for(char *i = key; *i; i++) {
		value = value + *i;
	}
	return value;
}

// Creates a new HashMap.
HashMap* create_hashmap(size_t key_space) {
	assert(key_space);
	HashMap *map = malloc(sizeof(HashMap));
	map->key_space = key_space;
	map->buckets = calloc(key_space, sizeof(Node*));
	map->hash = &default_hash;
	return map;
}

// Creates a new Node.
Node* create_node(char *key, void *val) {
	Node *new_node = malloc(sizeof(Node));
	new_node->next = NULL;
	new_node->key = strdup(key);
	new_node->value = val;
	return new_node;
}

// Insets an element into the HashMap, using the provided CollisionCallback function.
void insert_data(HashMap *hm, char *key, void *data, ResolveCollisionCallback resolve_collision) {
	assert(hm);
	assert(key);
	assert(resolve_collision);
	Hash hash = hm->hash(key) % hm->key_space;
	Node *pNode = *(hm->buckets+hash);
	while(pNode) {
		// Key was found.
		if (strcmp(pNode->key, key) == 0) {
			pNode->value = resolve_collision(pNode->value, data);
			return;
		}
		// End of linked list is reached (end of bucket).
		if (pNode->next == NULL) {
			pNode->next = create_node(key, data);
			if (pNode->next) {
			}
			return;
		}
		pNode = pNode->next;
	}
	// While was exited, bucket did not exist yet.
	*(hm->buckets+hash) = create_node(key, data);
}

void *get_data(HashMap *hm, char *key) {
	assert(hm);
	assert(key);	
	Hash hash = hm->hash(key) % hm->key_space;
	Node *pNode = *(hm->buckets+hash);
	while(pNode) {
		if (strcmp(pNode->key, key) == 0) {
			return pNode->value;
		}
		if (pNode->next == NULL) {
			break;
		}
		pNode = pNode->next;
	}
	return NULL;
}

void iterate(HashMap *hm, void (*callback)(char*,void*)) {
	assert(hm);
	assert(callback);
	for(Node **pBucket = hm->buckets; pBucket < hm->buckets+hm->key_space; pBucket++) {
		Node *pNode = *pBucket;
		while(pNode) {
			callback(pNode->key, pNode->value);
			pNode = pNode->next;
		}
	} 
}

void remove_data(HashMap *hm, char *key, DestroyDataCallback destroy_data) {
	assert(hm);
	assert(key);
	Hash hash = hm->hash(key) % hm->key_space;
	Node *pNode = *(hm->buckets+hash);
	// Save the previously visited node to relink the linked list.
	Node *pLast = NULL;
	while (pNode) {
		if (strcmp(pNode->key, key) == 0) {			
			// If pLast is still NULL, the root of the bucket / linked list must be removed.
			if (pLast == NULL) {
				*(hm->buckets+hash) = pNode->next;
			} else { // If it's not, the entry is in the middle / end.
				pLast->next = pNode->next;
			}
			if (destroy_data != NULL) {
				destroy_data(pNode->value);
			}
			free(pNode->key);
			free(pNode);
			return;
		}
		pLast = pNode;
		pNode = pNode->next;
	}
}

void delete_hashmap(HashMap *hm, DestroyDataCallback destroy_data) {
	assert(hm);
	for(Node **pBucket = hm->buckets; pBucket < hm->buckets+hm->key_space; pBucket++) {
		Node *pNode = *pBucket;
		Node *pNext;
		while(pNode) {
			pNext = pNode->next;
			if (destroy_data != NULL) {
				destroy_data(pNode->value);
			}
			free(pNode->key);
			free(pNode);
			pNode = pNext;
		}
	}
	free(hm->buckets);
	free(hm);
}
