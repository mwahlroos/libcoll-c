/*
 * hashmap.h
 *
 * a basic implementation of a hashmap
 */

#include <stdlib.h>
#include "linkedlist.h"
#include "node.h"

#ifndef CCOLL_HASHMAP_H
#define CCOLL_HASHMAP_H

typedef struct ccoccoll_ll_hm_entry {
    void *key;
    void *value;
} ccoccoll_ll_hm_entry_t;

typedef struct ccoccoll_ll_hashmap {
    ccoll_linkedlist_t **hash_slots;
    size_t load;
    size_t capacity;
    size_t total_entries;
    double max_load_factor;
    unsigned long (*hash_value_function)(void *key);
    int (*key_comparator_function)(void *key1, void *key2);
    int (*value_comparator_function)(void *value1, void *value2);
} ccoccoll_ll_hashmap_t;

ccoccoll_ll_hashmap_t* ccoccoll_ll_hm_init();

ccoccoll_ll_hashmap_t* ccoccoll_ll_hm_init_with_params(size_t init_capacity,
                                 double max_load_factor,
                                 unsigned long (*hash_value_function)(void*),
                                 int (*key_comparator_function)(void *key1, void *key2),
                                 int (*value_comparator_function)(void *value1, void *value2));

void ccoccoll_ll_hm_deinit(ccoccoll_ll_hashmap_t *hm);

void ccoccoll_ll_hm_put(ccoccoll_ll_hashmap_t *hm, void *key, void *value);

void* ccoccoll_ll_hm_get(ccoccoll_ll_hashmap_t *hm, void *key);

char ccoccoll_ll_hm_contains(ccoccoll_ll_hashmap_t *hm, void *key);

void* ccoccoll_ll_hm_remove(ccoccoll_ll_hashmap_t *hm, void *key);

size_t ccoccoll_ll_hm_get_capacity(ccoccoll_ll_hashmap_t *hm);

size_t ccoccoll_ll_hm_get_size(ccoccoll_ll_hashmap_t *hm);

#endif  /* CCOLL_HASHMAP_H */
