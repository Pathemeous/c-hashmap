#include <stddef.h>

typedef struct _HashMap HashMap;

typedef void*(*ResolveCollisionCallback)(void*,void*);

typedef void(*DestroyDataCallback)(void*);

HashMap* create_hashmap(size_t key_space);

void insert_data(HashMap *hm, char *key, void *data, ResolveCollisionCallback resolve_collision);

void *get_data(HashMap *hm, char *key);

void iterate(HashMap *hm, void (*callback)(char*,void*));

void remove_data(HashMap *hm, char *key, DestroyDataCallback destroy_data);

void delete_hashmap(HashMap *hm, DestroyDataCallback destroy_data);
