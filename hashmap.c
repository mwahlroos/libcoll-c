/*
 * hashmap.c
 */

#include <stdlib.h>
#include "hashmap.h"

#define HASHMAP_DEFAULT_INIT_SIZE         100
#define HASHMAP_DEFAULT_MAX_LOAD_FACTOR   0.75

static unsigned long hash(unsigned long key_value)
{
    /* FIXME: stub */
    return key_value;
}

static ccoll_linkedlist_t* find_collision_list(ccoccoll_ll_hashmap_t *hm, void *key)
{
    unsigned long key_hash = hash(hm->hash_value_function(key));
    size_t slot_index = (key_hash % hm->capacity);
    ccoll_linkedlist_t *collision_list = (*hm->hash_slots) + slot_index;

    return collision_list;
}

static ccoccoll_ll_hm_entry_t* find_entry(ccoccoll_ll_hashmap_t *hm, void *key)
{
    ccoccoll_ll_hm_entry_t *matching_entry = NULL;
    ccoll_linkedlist_t *collision_list = find_collision_list(hm, key);

    if (NULL != collision_list) {
        ccoll_ll_iter_t *iter = ccoll_ll_get_iter(collision_list);
        while (ccoll_ll_iter_has_next(iter)) {
            ccoccoll_ll_hm_entry_t *entry_tmp;
            void *key_tmp;

            ccoll_ll_node_t *node = ccoll_ll_iter_next(iter);
            entry_tmp = (ccoccoll_ll_hm_entry_t*) (node->value);
            key_tmp = entry_tmp->key;

            if (hm->key_comparator_function(key, key_tmp) == 0) {
                matching_entry = entry_tmp;
                break;
            }
        }
        ccoll_ll_drop_iter(iter);
    }

    return matching_entry;
}


ccoccoll_ll_hashmap_t* ccoccoll_ll_hm_init()
{
    return ccoccoll_ll_hm_init_with_params(HASHMAP_DEFAULT_INIT_SIZE,
                                     HASHMAP_DEFAULT_MAX_LOAD_FACTOR,
                                     NULL, NULL, NULL);
}

ccoccoll_ll_hashmap_t* ccoccoll_ll_hm_init_with_params(size_t init_capacity,
                                           double max_load_factor,
                                           unsigned long (*hash_value_function)(void*),
                                           int (*key_comparator_function)(void *key1, void *key2),
                                           int (*value_comparator_function)(void *value1, void *value2))
{
    ccoccoll_ll_hashmap_t *hm = (ccoccoll_ll_hashmap_t*) malloc(sizeof(ccoccoll_ll_hashmap_t));

    /* calloc automatically sets the entire allocated memory to zeros/NULLs,
     * which is useful in this case since it means unused buckets are
     * guaranteed to contain NULLs
     */
//    ccoll_linkedlist_t *first_slot = (ccoll_linkedlist_t*) calloc(init_capacity, sizeof(ccoll_linkedlist_t*));
    ccoll_linkedlist_t **buckets = (ccoll_linkedlist_t**) calloc(init_capacity, sizeof(ccoll_linkedlist_t*));

    hm->hash_slots = buckets;
    hm->max_load_factor = max_load_factor;
    hm->capacity = init_capacity;
    hm->load = 0;
    hm->total_entries = 0;

    if (NULL != hash_value_function) {
        hm->hash_value_function = hash_value_function;
    } else {
        hm->hash_value_function = &_ccoll_node_hashvalue_memaddr;
    }
    
    if (NULL != key_comparator_function) {
        hm->key_comparator_function = key_comparator_function;
    } else {
        hm->key_comparator_function = &_ccoll_node_comparator_memaddr;
    }

    if (NULL != value_comparator_function) {
        hm->value_comparator_function = value_comparator_function;
    } else {
        hm->value_comparator_function = &_ccoll_node_comparator_memaddr;
    }

    return hm;
}

void ccoccoll_ll_hm_deinit(ccoccoll_ll_hashmap_t *hm)
{
    for (size_t i=0; i<hm->capacity; i++) {
        ccoll_linkedlist_t *list = hm->hash_slots[i];
        /* FIXME: free key-value pair entries (hm_entry_t) too */
        if (NULL != list) {
            free(list);
        }
    }
    free(hm->hash_slots);
    free(hm);
}

void ccoccoll_ll_hm_put(ccoccoll_ll_hashmap_t *hm, void *key, void *value)
{
    unsigned long key_hash = hash(hm->hash_value_function(key));
    size_t slot_index = (key_hash % hm->capacity);
    ccoll_linkedlist_t *collision_list = hm->hash_slots[slot_index];

    if (NULL == collision_list) {
        collision_list = hm->hash_slots[slot_index]
                       = ccoll_ll_init_with_comparator(hm->key_comparator_function);
        hm->load++;
    }

    ccoccoll_ll_hm_entry_t *existing_entry = find_entry(hm, key);

    if (NULL != existing_entry) {
        /* replace the value in the existing entry with the new value */
        existing_entry->value = value;
    } else {
        ccoccoll_ll_hm_entry_t *new_kv_pair = (ccoccoll_ll_hm_entry_t*) malloc (sizeof(ccoccoll_ll_hm_entry_t));
        ccoll_ll_append(collision_list, new_kv_pair);
        hm->total_entries++;
    }
}

void* ccoccoll_ll_hm_get(ccoccoll_ll_hashmap_t *hm, void *key)
{
    void *value = NULL;
    ccoccoll_ll_hm_entry_t *entry = find_entry(hm, key);

    if (NULL != entry) {
        value = entry->value;
    }

    return value;
}

char ccoccoll_ll_hm_contains(ccoccoll_ll_hashmap_t *hm, void *key)
{
    ccoccoll_ll_hm_entry_t *entry = find_entry(hm, key);
    return NULL != entry;
}

void* ccoccoll_ll_hm_remove(ccoccoll_ll_hashmap_t *hm, void *key)
{
    ccoccoll_ll_hm_entry_t *kv_pair;
    void *retval = NULL;

    unsigned long key_hash = hash(hm->hash_value_function(key));
    size_t slot_index = (key_hash % hm->capacity);
    ccoll_linkedlist_t *collision_list = hm->hash_slots[slot_index];

    if (NULL != collision_list) {
        ccoll_ll_iter_t *iter = ccoll_ll_get_iter(collision_list);
        while (ccoll_ll_iter_has_next(iter)) {
            ccoll_ll_node_t *node = ccoll_ll_iter_next(iter);
            kv_pair = (ccoccoll_ll_hm_entry_t*) (node->value);
            if (hm->key_comparator_function(key, kv_pair->key) == 0) {
                retval = kv_pair->value;
                ccoll_ll_iter_remove(iter);
                hm->total_entries--;
            }
        }
    }

    /* FIXME: should perhaps indicate non-existence of the key somehow
     * rather than just returning NULL?
     */

    return retval;
}

size_t ccoccoll_ll_hm_get_capacity(ccoccoll_ll_hashmap_t *hm)
{
    return hm->capacity;
}

size_t ccoccoll_ll_hm_get_size(ccoccoll_ll_hashmap_t *hm)
{
    return hm->total_entries;
}
