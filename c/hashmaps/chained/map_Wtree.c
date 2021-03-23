/* Author: Marius Ingebrigtsen */
#include "../map.h"
#include "../../rbt/rbt.h"

#define INITIALSIZE 4096


/* Map Item Structure: */
typedef struct map_item map_item_t;
struct map_item {
	void			*key, *value;
	unsigned long	hashv;
};

/* Map Structure: */
struct map {
	rbt_t		**table;
	int			entries, maxentries;
	cmpfunc_t	cmpfunc;
	hashfunc_t	hashfunc;
};


/* Map Create: */
map_t *map_create(cmpfunc_t cmpfunc, hashfunc_t hashfunc) {
	map_t *map;

	map = calloc(1, sizeof(map_t));
	if(map == NULL) {
		fatal_error("Out of memory.\n");
	}
	map->table = calloc(INITIALSIZE, sizeof(rbt_t*));
	if(map->table == NULL) {
		fatal_error("Out of memory.\n");
	}
	map->maxentries	= INITIALSIZE;
	map->cmpfunc	= cmpfunc;
	map->hashfunc	= hashfunc;

	return map;
}

/* Map Destroy: */
static freefunc_t free_entry_value;	/* Global function-pointers for 'free_entry()' reference. */

static void free_entry(map_item_t *entry) {

	free_entry_value(entry->value);
	free(entry);
}

static void map_destroy_keys_values(map_t *map, freefunc_t freekey, freefunc_t freevalue) {
	/* Four types of allocations:
	 * Map, map-table, table-ADTs, ADTs' entry-contents. */
	
	free_entry_value = freevalue;	/* Assign function-pointers to global variables for further use in 'free_entry()'. */

	for(int indx = 0; indx < map->maxentries; indx++) {
		if(map->table[indx] != NULL) {
			rbt_destroy(map->table[indx], freekey, (freefunc_t)free_entry);	/* Instruct rbt-ADT to deallocate keys, entries and their values. */
		}
	}
	free(map->table);
	free(map);
}

static void map_destroy_keys(map_t *map, freefunc_t freekey) {
	/* Four types of allocations:
	 * Map, map-table, table-ADTs, ADTs' entry-contents. */
	
	for(int indx = 0; indx < map->maxentries; indx++) {
		if(map->table[indx] != NULL) {
			rbt_destroy(map->table[indx], freekey, free);	/* Instruct rbt-ADT to deallocate keys and entries. */
		}
	}
	free(map->table);
	free(map);
}

static void map_destroy_values(map_t *map, freefunc_t freevalue) {
	/* Four types of allocations:
	 * Map, map-table, table-ADTs, ADTs' entry-contents. */

	free_entry_value = freevalue;	/* Assign function-pointer to global variable for 'free_entry()'-reference. */

	for(int indx = 0; indx < map->maxentries; indx++) {
		if(map->table[indx] != NULL) {
			rbt_destroy(map->table[indx], NULL, (freefunc_t)free_entry);	/* Instruct rbt-ADT to deallocate entries and their values. */
		}
	}
	free(map->table);
	free(map);
}

static void map_destroy_map(map_t *map) {
	/* Four types of allocations:
	 * Map, map-table, table-ADTs, ADTs' entry-contents. */

	for(int indx = 0; indx < map->maxentries; indx++) {
		if(map->table[indx] != NULL) {
			rbt_destroy(map->table[indx], NULL, free);	/* Instruct rbt-ADT to deallocate entries. */
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
static map_item_t *entry_create(void *key, void *value, unsigned long hashv) {
	map_item_t *item;

	item = calloc(1, sizeof(map_item_t));
	if(item == NULL) {
		fatal_error("Out of memory.\n");
	}
	item->key	= key;
	item->value	= value;
	item->hashv	= hashv;

	return item;
}

static int map_resizeput(map_t *map, void *key, void *value) {
	/* Since call to this function only happen during resizing; 
	 * No check for size needed, no check for collisions is needed. */
	unsigned long	hashv;
	int				indx;

	hashv	= map->hashfunc(key);
	indx	= hashv % map->maxentries;

	if(map->table[indx] == NULL) {
		map->table[indx] = rbt_create(map->cmpfunc);
	}
	rbt_insert(map->table[indx], key, entry_create(key, value, hashv) );
	map->entries++;

	return 1;
}

static void map_resize(map_t *map) {
	rbt_t			**oldtable;
	int				oldsize;
	rbt_iterator_t	*iterator;
	map_item_t		*item;

	oldsize	= map->maxentries;
	oldtable= map->table;

	map->entries	= 0;
	map->maxentries	*= 2;
	map->table = calloc(map->maxentries, sizeof(rbt_t*));
	if(map->table == NULL) {
		fatal_error("Out of memory.\n");
	}

	for(int indx = 0; indx < oldsize; indx++) {
		if(oldtable[indx] != NULL) {
			iterator = rbt_createiterator(oldtable[indx]);
			
			while( (item = rbt_next(iterator)) ) {

				/* Could do map_put(), but following is more optimized. */
				map_resizeput(map, item->key, item->value);

				// free(item);	/* Deallocation of item-entry happen in rbt_destroy, below. */
			}
			rbt_destroyiterator(iterator);
			rbt_destroy(oldtable[indx], NULL, free);
		}
	}
	free(oldtable);
}

int map_put(map_t *map, void *key, void *value) {
	unsigned long	hashv;
	int				indx;
	rbt_iterator_t	*iterator;
	map_item_t		*item;

	if(map->entries >= map->maxentries) {
		map_resize(map);
	}

	hashv	= map->hashfunc(key);
	indx	= hashv % map->maxentries;

	if(map->table[indx] == NULL) {
		
		map->table[indx] = rbt_create(map->cmpfunc);
		rbt_insert(map->table[indx], key, entry_create(key, value, hashv) );

	} else {

		iterator = rbt_createiterator(map->table[indx]);
		while( (item = rbt_next(iterator)) ) {
			if( (hashv == item->hashv) && 
				(map->cmpfunc(key, item->key) == 0) ){

				item->value = value;
				break;
			}
		}
		rbt_destroyiterator(iterator);

		if(item == NULL) {
			rbt_insert(map->table[indx], key, entry_create(key, value, hashv) );
		} else {
			return 0;
		}
	}
	map->entries++;

	return 1;
}

/* Map Has Key: */
int map_haskey(map_t *map, void *key) {
	unsigned long	hashv;
	int				indx;
	rbt_iterator_t	*iterator;
	map_item_t		*item;

	hashv	= map->hashfunc(key);
	indx	= hashv % map->maxentries;

	if(map->table[indx] == NULL) {
		return 0;
	}
	iterator = rbt_createiterator(map->table[indx]);

	while( (item = rbt_next(iterator)) ) {
		if( (hashv == item->hashv) && 
			(map->cmpfunc(key, item->key) == 0) ) {

			break;
		}
	}
	rbt_destroyiterator(iterator);

	if(item == NULL) {
		return 0;
	}
	return 1;
}

/* Map Get: */
void *map_get(map_t *map, void *key) {
	unsigned long	hashv;
	int				indx;
	rbt_iterator_t	*iterator;
	map_item_t		*item;

	hashv	= map->hashfunc(key);
	indx	= hashv % map->maxentries;

	if(map->table[indx] == NULL) {
		return NULL;
	}
	iterator = rbt_createiterator(map->table[indx]);

	while( (item = rbt_next(iterator)) ) {
		if( (hashv == item->hashv) && 
			(map->cmpfunc(key, item->key) == 0) ) {

			break;
		}
	}
	rbt_destroyiterator(iterator);

	if(item == NULL) {
		return NULL;
	}
	return item->value;
}