/*
 * hashtable.c
 */

#include <stdlib.h>
#include "hashtable.h"

#define HASHTABLE_DEFAULT_INIT_SIZE     100
#define HASHTABLE_DEFAULT_LOAD_FACTOR   0.75

hashtable_t* ht_init()
{
    return ht_init_with_params(HASHTABLE_DEFAULT_INIT_SIZE, NULL, NULL, NULL);
}

hashtable_t* ht_init_with_params(size_t init_capacity,
                                 unsigned int (*hash_value_function)(void*),
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

int ht_add(hashtable_t *ht, void *key, void *value)
{   
    /* FIXME: make up something better than plain modulo? */
    size_t slot_index;
    
    linkedlist_t *collision_list;
    ht_key_value_pair_t *pair;
    void *list_node_value;
    
    slot_index = (ht->hash_value_function(key) % ht->capacity);
    collision_list = ht->hash_slots[slot_index];

    if (NULL == collision_list) {
        collision_list = ht->hash_slots[slot_index] = ll_init(ht->key_comparator_function);
    }
    pair = (ht_key_value_pair_t*) malloc (sizeof(ht_key_value_pair_t));
    list_node_value = pair;
    ll_append(collision_list, list_node_value);

    ht->size++;

    return 0;
}

void* ht_get(hashtable_t *ht, void *key)
{
    /* FIXME: stub */
    return NULL;
}

void* ht_remove(hashtable_t *ht, void *key)
{
    /* FIXME: stub */
    return NULL;
}

size_t ht_get_capacity(hashtable_t *ht)
{
    return ht->capacity;
}

size_t ht_get_size(hashtable_t *ht)
{
    return ht->size;
}
