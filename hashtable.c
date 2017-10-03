/*
 * hashtable.c
 */

#include <stdlib.h>
#include "hashtable.h"

#define HASHTABLE_DEFAULT_INIT_SIZE         100
#define HASHTABLE_DEFAULT_MAX_LOAD_FACTOR   0.75

static unsigned long hash(unsigned long key_value)
{
    /* FIXME: stub */
    return key_value;
}

static linkedlist_t* find_collision_list(hashtable_t *ht, void *key)
{
    unsigned long key_hash = hash(ht->hash_value_function(key));
    size_t slot_index = (key_hash % ht->capacity);
    linkedlist_t *collision_list = (*ht->hash_slots) + slot_index;

    return collision_list;
}

static ht_key_value_pair_t* find_entry(hashtable_t *ht, void *key)
{
    ht_key_value_pair_t *matching_entry = NULL;
    linkedlist_t *collision_list = find_collision_list(ht, key);

    if (NULL != collision_list) {
        ll_iter_t *iter = ll_get_iter(collision_list);
        while (ll_has_next(iter)) {
            ht_key_value_pair_t *entry_tmp;
            void *key_tmp;

            ll_node_t *node = ll_next(iter);
            entry_tmp = (ht_key_value_pair_t*) (node->value);
            key_tmp = entry_tmp->key;

            if (ht->key_comparator_function(key, key_tmp) == 0) {
                matching_entry = entry_tmp;
                break;
            }
        }
        ll_drop_iter(iter);
    }

    return matching_entry;
}


hashtable_t* ht_init()
{
    return ht_init_with_params(HASHTABLE_DEFAULT_INIT_SIZE,
                               HASHTABLE_DEFAULT_MAX_LOAD_FACTOR,
                               NULL, NULL, NULL);
}

hashtable_t* ht_init_with_params(size_t init_capacity,
                                 double max_load_factor,
                                 unsigned long (*hash_value_function)(void*),
                                 int (*key_comparator_function)(void *key1, void *key2),
                                 int (*value_comparator_function)(void *value1, void *value2))
{
    hashtable_t *ht = (hashtable_t*) malloc(sizeof(hashtable_t));

    /* calloc automatically sets the entire allocated memory to zeros/NULLs,
     * which is useful in this case since it means unused buckets are
     * guaranteed to contain NULLs
     */
//    linkedlist_t *first_slot = (linkedlist_t*) calloc(init_capacity, sizeof(linkedlist_t*));
    linkedlist_t **buckets = (linkedlist_t**) calloc(init_capacity, sizeof(linkedlist_t*));

    ht->hash_slots = buckets;
    ht->max_load_factor = max_load_factor;
    ht->capacity = init_capacity;
    ht->load = 0;
    ht->total_entries = 0;

    if (NULL != hash_value_function) {
        ht->hash_value_function = hash_value_function;
    } else {
        ht->hash_value_function = &_node_hashvalue_memaddr;
    }
    
    if (NULL != key_comparator_function) {
        ht->key_comparator_function = key_comparator_function;
    } else {
        ht->key_comparator_function = &_node_comparator_memaddr;
    }

    if (NULL != value_comparator_function) {
        ht->value_comparator_function = value_comparator_function;
    } else {
        ht->value_comparator_function = &_node_comparator_memaddr;
    }

    return ht;
}

void ht_deinit(hashtable_t *ht)
{
    for (size_t i=0; i<ht->capacity; i++) {
        linkedlist_t *list = ht->hash_slots[i];
        /* FIXME: free key-value pair entries (ht_key_value_pair_t) too */
        if (NULL != list) {
            free(list);
        }
    }
    free(ht->hash_slots);
    free(ht);
}

void ht_put(hashtable_t *ht, void *key, void *value)
{
    unsigned long key_hash = hash(ht->hash_value_function(key));
    size_t slot_index = (key_hash % ht->capacity);
    linkedlist_t *collision_list = ht->hash_slots[slot_index];

    if (NULL == collision_list) {
        collision_list = ht->hash_slots[slot_index]
                       = ll_init_with_comparator(ht->key_comparator_function);
        ht->load++;
    }

    ht_key_value_pair_t *existing_entry = find_entry(ht, key);

    if (NULL != existing_entry) {
        /* replace the value in the existing entry with the new value */
        existing_entry->value = value;
    } else {
        ht_key_value_pair_t *new_kv_pair = (ht_key_value_pair_t*) malloc (sizeof(ht_key_value_pair_t));
        ll_append(collision_list, new_kv_pair);
        ht->total_entries++;
    }
}

void* ht_get(hashtable_t *ht, void *key)
{
    void *value = NULL;
    ht_key_value_pair_t *entry = find_entry(ht, key);

    if (NULL != entry) {
        value = entry->value;
    }

    return value;
}

void* ht_remove(hashtable_t *ht, void *key)
{
    ht_key_value_pair_t *kv_pair;
    void *retval = NULL;

    unsigned long key_hash = hash(ht->hash_value_function(key));
    size_t slot_index = (key_hash % ht->capacity);
    linkedlist_t *collision_list = ht->hash_slots[slot_index];

    if (NULL != collision_list) {
        ll_iter_t *iter = ll_get_iter(collision_list);
        while (ll_has_next(iter)) {
            ll_node_t *node = ll_next(iter);
            kv_pair = (ht_key_value_pair_t*) (node->value);
            if (ht->key_comparator_function(key, kv_pair->key) == 0) {
                retval = kv_pair->value;
                ll_remove_last_returned(iter);
                ht->total_entries--;
            }
        }
    }

    /* FIXME: should perhaps indicate non-existence of the key somehow
     * rather than just returning NULL?
     */

    return retval;
}

size_t ht_get_capacity(hashtable_t *ht)
{
    return ht->capacity;
}

size_t ht_get_size(hashtable_t *ht)
{
    return ht->total_entries;
}
