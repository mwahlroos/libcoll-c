/*
 * hashmap.h
 *
 * a basic implementation of a hashmap
 */

#include <stdlib.h>
#include "linkedlist.h"
#include "map.h"
#include "node.h"
#include "types.h"

#ifndef LIBCOLL_HASHMAP_H
#define LIBCOLL_HASHMAP_H

#define LIBCOLL_HASHMAP_DEFAULT_INIT_SIZE         32
#define LIBCOLL_HASHMAP_DEFAULT_MAX_LOAD_FACTOR   0.75f

typedef struct libcoll_hashmap_entry {
    void *key;
    void *value;
} libcoll_hashmap_entry_t;

typedef struct libcoll_hashmap {
    libcoll_linkedlist_t **buckets;
    size_t capacity;
    size_t total_entries;
    float max_load_factor;
    unsigned long (*hash_code_function)(void *key);
    int (*key_comparator_function)(void *key1, void *key2);
    int (*value_comparator_function)(void *value1, void *value2);
} libcoll_hashmap_t;

libcoll_hashmap_t* libcoll_hashmap_init();

libcoll_hashmap_t* libcoll_hashmap_init_with_params(size_t init_capacity,
                                 float max_load_factor,
                                 unsigned long (*hash_code_function)(void*),
                                 int (*key_comparator_function)(void *key1, void *key2),
                                 int (*value_comparator_function)(void *value1, void *value2));

void libcoll_hashmap_deinit(libcoll_hashmap_t *hm);

libcoll_map_insertion_result_t libcoll_hashmap_put(libcoll_hashmap_t *hm, void *key, void *value);

void* libcoll_hashmap_get(libcoll_hashmap_t *hm, void *key);

char libcoll_hashmap_contains(libcoll_hashmap_t *hm, void *key);

libcoll_map_removal_result_t libcoll_hashmap_remove(libcoll_hashmap_t *hm, void *key);

size_t libcoll_hashmap_get_capacity(libcoll_hashmap_t *hm);

size_t libcoll_hashmap_get_size(libcoll_hashmap_t *hm);

char libcoll_hashmap_is_empty(libcoll_hashmap_t *hm);

#endif  /* LIBCOLL_HASHMAP_H */
