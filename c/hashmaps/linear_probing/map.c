/* Author: Marius Ingebrigtsen */
/* Hashmap implementation is Linear Probing / Open Addressing. */
#include "../map.h"

#define INITIAL_SIZE 3204


typedef struct map_item {
	void			*key, *value;
	unsigned long	hashv;
} map_item_t;

struct map {
	map_item_t	*table;
	int			numitems, maxitems;
	cmpfunc_t	cmpfunc;
	hashfunc_t	hashfunc;
};


map_t *map_create(cmpfunc_t cmpfunc, hashfunc_t hashfunc) {
	map_t *map = (map_t*)malloc(sizeof(map_t));
	if(map == NULL) {
		fatal_error("Out of memory.\n");
	}
	map->table = (map_item_t*)calloc(INITIAL_SIZE, sizeof(map_item_t));
	if(map->table == NULL) {
		fatal_error("Out of memory.\n");
	}
	map->numitems	= 0;
	map->maxitems	= INITIAL_SIZE;
	map->cmpfunc	= cmpfunc;
	map->hashfunc	= hashfunc;
	
	return map;
}

static void map_destroy_keys_values(map_t *map, freefunc_t freekey, freefunc_t freevalue) {
	
	for(int i = 0; i < map->maxitems; i++) {
		if(map->table[i].key != NULL) {
			freekey(map->table[i].key);
		}
		if(map->table[i].value != NULL) {
			freevalue(map->table[i].value);
		}
	}
}

static void map_destroy_keys(map_t *map, freefunc_t freekey) {
	
	for(int i = 0; i < map->maxitems; i++) {
		if(map->table[i].key != NULL) {
			freekey(map->table[i].key);
		}
	}
}

static void map_destroy_values(map_t *map, freefunc_t freevalue) {
	
	for(int i = 0; i < map->maxitems; i++) {
		if(map->table[i].value != NULL) {
			freevalue(map->table[i].value);
		}
	}
}

void map_destroy(map_t *map, freefunc_t freekey, freefunc_t freevalue) {
	
	if( (freekey != NULL) && (freevalue != NULL) ) {		/* If both keys and values are to be destroyed. */
		map_destroy_keys_values(map, freekey, freevalue);
	} 
	else if(freekey != NULL) {	/* If only keys are to be destroyed. */
		map_destroy_keys(map, freekey);
	}
	else if(freevalue != NULL) {	/* If only values are to be destroyed. */
		map_destroy_values(map, freevalue);
	}

	free(map->table);
	free(map);
}

int map_size(map_t *map) {
	return map->numitems;
}

static inline void map_resize(map_t *map) {
	map_item_t	*old_map, *new_map;
	int			old_size, new_size;

	old_size		= map->maxitems;
	old_map			= map->table;
	
	new_size		= 2*map->maxitems;
	new_map			= (map_item_t*)calloc(new_size, sizeof(map_item_t));
	if(new_map == NULL) {
		fatal_error("Out of memory.\n");
	}
	map->table		= new_map;
	map->maxitems	= new_size;
	map->numitems	= 0;
	
	for(int i = 0; i < old_size; i++) {
		if(old_map[i].key != NULL) {
			map_put(map, old_map[i].key, old_map[i].value);
		}
	}
	free(old_map);
}

int map_put(map_t *map, void *key, void *value) {
	unsigned long hashv; 
	int indx;
	
	if(map->numitems >= map->maxitems / 2) {
		map_resize(map);
	}
	hashv = map->hashfunc(key);
	
	for(indx = hashv % map->maxitems;	
		map->table[indx].key != NULL;
		indx = (indx + 1) % map->maxitems) {
		
		if( (hashv == map->table[indx].hashv) && 
			(map->cmpfunc(key, map->table[indx].key) == 0) ) {
			map->table[indx].value = value;
			return 0;
		}
	}
	map->table[indx].key	= key;
	map->table[indx].value	= value;
	map->table[indx].hashv	= hashv;
	map->numitems++;

	return 1;
}

int map_haskey(map_t *map, void *key) {
	unsigned long hashv; int indx;

	hashv = map->hashfunc(key);
	
	for(indx = hashv % map->maxitems;
		map->table[indx].key != NULL;
		indx = (indx + 1) % map->maxitems) {
	
		if( (map->table[indx].hashv == hashv) &&
			(map->cmpfunc(map->table[indx].key, key) == 0) ) {
			
			return 1;
		}
	}
	return 0;
}

void *map_get(map_t *map, void *key) {
	unsigned long hashv; int indx;
	
	hashv = map->hashfunc(key);

	for(indx = hashv % map->maxitems;
		map->table[indx].key != NULL;
		indx = (indx + 1) % map->maxitems) {
	
		if( (map->table[indx].hashv == hashv) && 
			(map->cmpfunc(map->table[indx].key, key) == 0) ) {
			
			return map->table[indx].value;
		}
	}

	return NULL;
}