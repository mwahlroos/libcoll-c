/*
 * hashtable.c
 */

#include <stdlib.h>
#include <stdbool.h>
#include "hashtable.h"

#define HASHTABLE_DEFAULT_INIT_SIZE     100
#define HASHTABLE_DEFAULT_LOAD_FACTOR   0.75

static unsigned long hash(unsigned long key_value)
{
    /* FIXME: stub */
    return key_value;
}

hashtable_t* ht_init()
{
    return ht_init_with_params(HASHTABLE_DEFAULT_INIT_SIZE, NULL, NULL, NULL);
}

hashtable_t* ht_init_with_params(size_t init_capacity,
                                 unsigned long (*hash_value_function)(void*),
                                 int (*key_comparator_function)(void *key1, void *key2),
                                 int (*value_comparator_function)(void *value1, void *value2))
{
    hashtable_t *ht = (hashtable_t*) malloc(sizeof(hashtable_t));

    linkedlist_t *first_slot = (linkedlist_t*) malloc(init_capacity * sizeof(linkedlist_t));
    ht->hash_slots = &first_slot;

    ht->capacity = init_capacity;
    ht->size = 0;

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
        collision_list = ht->hash_slots[slot_index] = ll_init(ht->key_comparator_function);
    }

    /* check for existing entries with a matching key */

    bool exists = false;

    ll_iter_t *iter = ll_get_iter(collision_list);
    while (ll_has_next(iter)) {
        ht_key_value_pair_t *kv_pair_tmp;
        void *key_tmp;

        ll_node_t *node = ll_next(iter);
        kv_pair_tmp = (ht_key_value_pair_t*) (node->value);
        key_tmp = kv_pair_tmp->key;

        if (ht->key_comparator_function(key, key_tmp) == 0) {
            /* replace the existing value with the new one */
            kv_pair_tmp->value = value;
            exists = true;
        }
    }

    if (!exists) {
        ht_key_value_pair_t *new_kv_pair = (ht_key_value_pair_t*) malloc (sizeof(ht_key_value_pair_t));
        ll_append(collision_list, new_kv_pair);
        ht->size++;
    }
}

void* ht_get(hashtable_t *ht, void *key)
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
                break;
            }
        }
    }

    return retval;
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
                ht->size--;
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
    return ht->size;
}
