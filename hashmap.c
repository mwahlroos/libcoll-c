/*
 * hashmap.c
 */

#include <stdlib.h>
#include "hashmap.h"
#include "debug.h"

#define HASHMAP_DEFAULT_INIT_SIZE         32
#define HASHMAP_DEFAULT_MAX_LOAD_FACTOR   0.75f

static size_t hash(ccoll_hashmap_t *hm, unsigned long hashcode)
{
    /* trivial distribution for now */
    return hashcode % hm->capacity;
}

static ccoll_linkedlist_t* find_collision_list(ccoll_hashmap_t *hm, void *key)
{
    size_t slot_index = hash(hm, hm->hash_code_function(key));
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

/*
 * Inserts the given key-value pair into the given collision list.
 * If a value with the same key already exists, it is replaced.
 *
 * Returns: an insertion result indicating whether an existing entry was
 * replaced or a new entry added, or whether there was an error.
 */
static ccoll_map_insertion_result_t insert_new(ccoll_hashmap_t *hm, void *key, void *value)
{
    ccoll_map_insertion_result_t result;

    if (NULL == key) {
        result.status = INSERTION_FAILED;
        result.error = INVALID_KEY;
        return result;
    }

    ccoll_hashmap_entry_t *new_entry = malloc(sizeof(ccoll_hashmap_entry_t));
    new_entry->key = key;
    new_entry->value = value;

    size_t slot_index = hash(hm, hm->hash_code_function(key));
    ccoll_linkedlist_t *collision_list = hm->hash_slots[slot_index];

    if (NULL == collision_list) {
        collision_list = ccoll_linkedlist_init_with_comparator(hm->key_comparator_function);
        hm->hash_slots[slot_index] = collision_list;
    }

    ccoll_linkedlist_iter_t *iter = ccoll_linkedlist_get_iter(collision_list);
    ccoll_linkedlist_node_t *node;
    char key_exists = 0;

    while ((node = ccoll_linkedlist_iter_next(iter)) != NULL) {
        ccoll_hashmap_entry_t *entry = (ccoll_hashmap_entry_t*) node->value;
        if (hm->key_comparator_function(key, entry->key) == 0) {
            result.old_key = entry->key;
            result.old_value = entry->value;
            result.status = REPLACED;

            key_exists = 1;

            free(entry);
            node->value = new_entry;
            break;
        }
    }
    ccoll_linkedlist_drop_iter(iter);

    if (!key_exists) {
        ccoll_linkedlist_append(collision_list, new_entry);
        result.status = ADDED;
    }

    return result;
}

ccoll_hashmap_t* ccoll_hashmap_init()
{
    return ccoll_hashmap_init_with_params(HASHMAP_DEFAULT_INIT_SIZE,
                                     HASHMAP_DEFAULT_MAX_LOAD_FACTOR,
                                     NULL, NULL, NULL);
}

ccoll_hashmap_t* ccoll_hashmap_init_with_params(size_t init_capacity,
                                           float max_load_factor,
                                           unsigned long (*hash_code_function)(void*),
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

    if (NULL != hash_code_function) {
        hm->hash_code_function = hash_code_function;
    } else {
        hm->hash_code_function = &_ccoll_node_hashvalue_memaddr;
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

ccoll_map_insertion_result_t ccoll_hashmap_put(ccoll_hashmap_t *hm, void *key, void *value)
{
    ccoll_map_insertion_result_t result = insert_new(hm, key, value);
    if (result.status == ADDED) {
        hm->total_entries++;
    }
    return result;
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

ccoll_map_removal_result_t ccoll_hashmap_remove(ccoll_hashmap_t *hm, void *key)
{
    ccoll_map_removal_result_t result;

    if (NULL == key) {
        result.status = REMOVAL_FAILED;
        result.error = INVALID_KEY;
        return result;
    }

    result.status = NOT_FOUND;
    result.key = NULL;
    result.value = NULL;

    unsigned long key_hash = hash(hm, hm->hash_code_function(key));
    size_t slot_index = (key_hash % hm->capacity);
    ccoll_linkedlist_t *collision_list = hm->hash_slots[slot_index];

    if (NULL != collision_list) {
        ccoll_linkedlist_iter_t *iter = ccoll_linkedlist_get_iter(collision_list);
        while (ccoll_linkedlist_iter_has_next(iter)) {
            ccoll_linkedlist_node_t *node = ccoll_linkedlist_iter_next(iter);
            ccoll_hashmap_entry_t *entry = (ccoll_hashmap_entry_t*) (node->value);
            if (hm->key_comparator_function(key, entry->key) == 0) {
                result.key = entry->key;
                result.value = entry->value;
                result.status = REMOVED;
                ccoll_linkedlist_iter_remove(iter);
                hm->total_entries--;
            }
        }
        ccoll_linkedlist_drop_iter(iter);
    }

    return result;
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
