/* Author: Marius Ingebrigtsen */
#include "../map.h"

#define INITIALSIZE 4096


/* Map Item Structure: */
typedef struct map_item map_item_t;
struct map_item {
	void			*key, *value;
	unsigned long	hashv;
	map_item_t		*next;
};

/* Map Structure: */
struct map {
	map_item_t	**table;
	int			entries, maxentries;
	cmpfunc_t	cmpfunc;
	hashfunc_t	hashfunc;
};


/* Map Create: */
map_t *map_create(cmpfunc_t cmpfunc, hashfunc_t hashfunc) {
	map_t *map;

	map = (map_t*)malloc(sizeof(map_t));
	if(map == NULL) {
		fatal_error("Out of memory.\n");
	}
	map->table		= (map_item_t**)calloc(INITIALSIZE, sizeof(map_item_t*));
	if(map->table == NULL) {
		fatal_error("Out of memory.\n");
	}
	map->entries	= 0;
	map->maxentries	= INITIALSIZE;
	map->cmpfunc	= cmpfunc;
	map->hashfunc	= hashfunc;

	return map;
}

/* Map Destroy: */
static void map_destroy_keys_values(map_t *map, freefunc_t freekey, freefunc_t freevalue) {
	map_item_t *item, *tmp;

	for(int i = 0; i < map->maxentries; i++) {
		item = map->table[i];

		while(item != NULL) {
			tmp = item;
			item = item->next;
			freekey(tmp->key);
			freevalue(tmp->value);
			free(tmp);
		}
	}
	free(map->table);
	free(map);
}

static void map_destroy_keys(map_t *map, freefunc_t freekey) {
	map_item_t *item, *tmp;

	for(int i = 0; i < map->maxentries; i++) {
		item = map->table[i];

		while(item != NULL) {
			tmp = item;
			item = item->next;
			freekey(tmp->key);
			free(tmp);
		}
	}
	free(map->table);
	free(map);
}

static void map_destroy_values(map_t *map, freefunc_t freevalue) {
	map_item_t *item, *tmp;

	for(int i = 0; i < map->maxentries; i++) {
		item = map->table[i];

		while(item != NULL) {
			tmp = item;
			item = item->next;
			freevalue(tmp->value);
			free(tmp);
		}
	}
	free(map->table);
	free(map);
}

static void map_destroy_map(map_t *map) {
	map_item_t *item, *tmp;

	for(int i = 0; i < map->maxentries; i++) {
		item = map->table[i];

		while(item != NULL) {
			tmp = item;
			item = item->next;
			free(tmp);
		}
	}
	free(map->table);
	free(map);
}

void map_destroy(map_t *map, freefunc_t freekey, freefunc_t freevalue) {
	
	if( (freekey != NULL) && (freevalue != NULL) ) {
		map_destroy_keys_values(map, freekey, freevalue);
	}
	else if(freekey != NULL) {
		map_destroy_keys(map, freekey);
	}
	else if(freevalue != NULL) {
		map_destroy_values(map, freevalue);
	}
	else {
		map_destroy_map(map);
	}
}

/* Map Size: */
int map_size(map_t *map) {
	return map->entries;
}

/* Map Put: */
static void map_resize(map_t *map) {
	map_item_t	**newtable, **oldtable, *item, *tmp;
	int			newsize, oldsize;

	oldsize = map->maxentries;
	oldtable = map->table;

	newsize = oldsize * 2;
	newtable = (map_item_t**)calloc(newsize, sizeof(map_item_t*));
	if(newtable == NULL) {
		fatal_error("Out of memory.\n");
	}

	map->entries = 0;
	map->maxentries = newsize;
	map->table = newtable;

	for(int i = 0; i < oldsize; i++) {
		item = oldtable[i];

		while(item != NULL) {
			map_put(map, item->key, item->value);
			
			tmp = item;
			
			item = item->next;
			
			free(tmp);
		}
	}
	free(oldtable);
}

static map_item_t *entry_create(void *key, void *value, unsigned long hashv) {
	map_item_t *item;

	item = (map_item_t*)malloc(sizeof(map_item_t));
	if(item == NULL) {
		fatal_error("Out of memory.\n");
	}
	item->key	= key;
	item->value	= value;
	item->hashv	= hashv;
	item->next	= NULL;

	return item;
}

int map_put(map_t *map, void *key, void *value) {
	unsigned long hashv;
	int indx;
	map_item_t *item;

	if(map->entries >= map->maxentries) {
		map_resize(map);
	}

	hashv = map->hashfunc(key);
	indx = hashv % map->maxentries;

	for(item = map->table[indx];
		item != NULL;
		item = item->next ) {
		if( (hashv == map->table[indx]->hashv) && 
			(map->cmpfunc(key, map->table[indx]->key) == 0) ) {
			
			map->table[indx]->value = value;
			return 0;
		} else if(item->next == NULL) {
			
			item->next = entry_create(key, value, hashv);
			map->entries++;
			break;
		}
	}

	if(item == NULL) {
		map->table[indx] = entry_create(key, value, hashv);
		map->entries++;
	}

	return 1;
}

/* Map Has Key: */
int map_haskey(map_t *map, void *key) {
	unsigned long hashv;
	int indx;
	map_item_t *item;

	hashv = map->hashfunc(key);

	indx = hashv % map->maxentries;

	item = map->table[indx];

	while(item != NULL) {
		if( (hashv == item->hashv) && 
			(map->cmpfunc(key, item->key) == 0) ) {
			break;
		}
		item = item->next;
	}
	if(item == NULL) {
		return 0;
	}
	return 1;
}

/* Map Get: */
void *map_get(map_t *map, void *key) {
	unsigned long hashv;
	int indx;
	map_item_t *item;

	hashv = map->hashfunc(key);

	indx = hashv % map->maxentries;

	item = map->table[indx];

	while(item != NULL) {
		if( (hashv == item->hashv) && 
			(map->cmpfunc(key, item->key) == 0) ) {
			return item->value;
		}
		item = item->next;
	}
	return NULL;
}
