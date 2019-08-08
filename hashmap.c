/*
 * hashmap.c
 */

#include <stdlib.h>
#include "hashmap.h"
#include "debug.h"

#define HASHMAP_DEFAULT_INIT_SIZE         100
#define HASHMAP_DEFAULT_MAX_LOAD_FACTOR   0.75

static unsigned long hash(unsigned long key_value)
{
    /* FIXME: stub */
    return key_value;
}

static ccoll_linkedlist_t* find_collision_list(ccoll_hashmap_t *hm, void *key)
{
    unsigned long key_hash = hash(hm->hash_value_function(key));
    size_t slot_index = (key_hash % hm->capacity);
    ccoll_linkedlist_t *collision_list = hm->hash_slots[slot_index];

    return collision_list;
}

static ccoll_hashmap_entry_t* find_entry(ccoll_hashmap_t *hm, void *key)
{
    ccoll_hashmap_entry_t *matching_entry = NULL;
    ccoll_linkedlist_t *collision_list = find_collision_list(hm, key);

    if (NULL != collision_list) {
        ccoll_linkedlist_iter_t *iter = ccoll_linkedlist_get_iter(collision_list);
        while (ccoll_linkedlist_iter_has_next(iter)) {
            ccoll_hashmap_entry_t *entry_tmp;
            void *key_tmp;

            ccoll_linkedlist_node_t *node = ccoll_linkedlist_iter_next(iter);
            entry_tmp = (ccoll_hashmap_entry_t*) (node->value);
            key_tmp = entry_tmp->key;

            if (hm->key_comparator_function(key, key_tmp) == 0) {
                matching_entry = entry_tmp;
                break;
            }
        }
        ccoll_linkedlist_drop_iter(iter);
    }

    return matching_entry;
}


ccoll_hashmap_t* ccoll_hashmap_init()
{
    return ccoll_hashmap_init_with_params(HASHMAP_DEFAULT_INIT_SIZE,
                                     HASHMAP_DEFAULT_MAX_LOAD_FACTOR,
                                     NULL, NULL, NULL);
}

ccoll_hashmap_t* ccoll_hashmap_init_with_params(size_t init_capacity,
                                           double max_load_factor,
                                           unsigned long (*hash_value_function)(void*),
                                           int (*key_comparator_function)(void *key1, void *key2),
                                           int (*value_comparator_function)(void *value1, void *value2))
{
    ccoll_hashmap_t *hm = (ccoll_hashmap_t*) malloc(sizeof(ccoll_hashmap_t));

    /* calloc automatically sets the entire allocated memory to zeros/NULLs,
     * which is useful in this case since it means unused buckets are
     * guaranteed to contain NULLs
     */
//    ccoll_linkedlist_t *first_slot = (ccoll_linkedlist_t*) calloc(init_capacity, sizeof(ccoll_linkedlist_t*));
    ccoll_linkedlist_t **buckets = (ccoll_linkedlist_t**) calloc(init_capacity, sizeof(ccoll_linkedlist_t*));

    hm->hash_slots = buckets;
    hm->max_load_factor = max_load_factor;
    hm->capacity = init_capacity;
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

void ccoll_hashmap_deinit(ccoll_hashmap_t *hm)
{
    for (size_t i=0; i<hm->capacity; i++) {
        ccoll_linkedlist_t *list = hm->hash_slots[i];
        if (NULL != list) {
            ccoll_linkedlist_iter_t *iter = ccoll_linkedlist_get_iter(list);
            while (ccoll_linkedlist_iter_has_next(iter)) {
                ccoll_hashmap_entry_t *entry = (ccoll_hashmap_entry_t*) ccoll_linkedlist_iter_next(iter);
                free(entry);
            }
            free(list);
        }
    }
    free(hm->hash_slots);
    free(hm);
}

void ccoll_hashmap_put(ccoll_hashmap_t *hm, void *key, void *value)
{
    unsigned long key_hash = hash(hm->hash_value_function(key));
    size_t slot_index = (key_hash % hm->capacity);
    ccoll_linkedlist_t *collision_list = hm->hash_slots[slot_index];

    if (NULL == collision_list) {
        collision_list = ccoll_linkedlist_init_with_comparator(hm->key_comparator_function);
        hm->hash_slots[slot_index] = collision_list;
    }

    ccoll_hashmap_entry_t *existing_entry = find_entry(hm, key);

    if (NULL != existing_entry) {
        /* replace the value in the existing entry with the new value */
        existing_entry->value = value;
    } else {
        ccoll_hashmap_entry_t *new_kv_pair = (ccoll_hashmap_entry_t*) malloc (sizeof(ccoll_hashmap_entry_t));
        new_kv_pair->key = key;
        new_kv_pair->value = value;

        ccoll_linkedlist_append(collision_list, new_kv_pair);
        hm->total_entries++;
    }
}

void* ccoll_hashmap_get(ccoll_hashmap_t *hm, void *key)
{
    void *value = NULL;
    ccoll_hashmap_entry_t *entry = find_entry(hm, key);

    if (NULL != entry) {
        value = entry->value;
    }

    return value;
}

char ccoll_hashmap_contains(ccoll_hashmap_t *hm, void *key)
{
    ccoll_hashmap_entry_t *entry = find_entry(hm, key);
    return NULL != entry;
}

ccoll_pair_voidptr_t ccoll_hashmap_remove(ccoll_hashmap_t *hm, void *key)
{
    ccoll_hashmap_entry_t *entry;
    ccoll_pair_voidptr_t kv_pair;

    unsigned long key_hash = hash(hm->hash_value_function(key));
    size_t slot_index = (key_hash % hm->capacity);
    ccoll_linkedlist_t *collision_list = hm->hash_slots[slot_index];

    if (NULL != collision_list) {
        ccoll_linkedlist_iter_t *iter = ccoll_linkedlist_get_iter(collision_list);
        while (ccoll_linkedlist_iter_has_next(iter)) {
            ccoll_linkedlist_node_t *node = ccoll_linkedlist_iter_next(iter);
            entry = (ccoll_hashmap_entry_t*) (node->value);
            if (hm->key_comparator_function(key, entry->key) == 0) {
                kv_pair.a = entry->key;
                kv_pair.b = entry->value;
                ccoll_linkedlist_iter_remove(iter);
                hm->total_entries--;
            }
        }
    }

    /* FIXME: should perhaps indicate non-existence of the key somehow
     * rather than just returning null pointers in the struct?
     */

    return kv_pair;
}

size_t ccoll_hashmap_get_capacity(ccoll_hashmap_t *hm)
{
    return hm->capacity;
}

size_t ccoll_hashmap_get_size(ccoll_hashmap_t *hm)
{
    return hm->total_entries;
}

char ccoll_hashmap_is_empty(ccoll_hashmap_t *hm)
{
    return hm->total_entries == 0;
}
