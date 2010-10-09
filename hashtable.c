/*
 * hashtable.c
 */

#include <stdlib.h>
#include "hashtable.h"

extern hashtable_t* ht_init(size_t init_capacity,
                            unsigned int (*hash_value_function)(node_value_t*),
                            int (*compare_function)(node_value_t *v1, node_value_t *v2))
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
    
    if (NULL != compare_function) {
        ht->compare_function = compare_function;
    } else {
        ht->compare_function = &_node_comparator_memaddr;
    }

    return ht;
}

extern void ht_deinit(hashtable_t *ht)
{
    /* TODO: deinit each non-null linkedlist? */
    free(ht->hash_slots);
    free(ht);
}

extern int ht_add(hashtable_t *ht, node_value_t *key, node_value_t *value)
{   
    /* FIXME: make up something better than plain modulo */
    size_t slot_index;
    
    linkedlist_t *collision_list;
    ht_key_value_pair_t *pair;
    node_value_t *list_node_value;
    
    slot_index = (ht->hash_value_function(key) % ht->capacity);
    collision_list = ht->hash_slots[slot_index];

    if (NULL == collision_list) {
        collision_list = ht->hash_slots[slot_index] = ll_init(ht->compare_function);
    }
    pair = (ht_key_value_pair_t*) malloc (sizeof(ht_key_value_pair_t));
    list_node_value = (node_value_t*) malloc (sizeof(node_value_t));
    list_node_value->payload = pair;
    list_node_value->size = sizeof(ht_key_value_pair_t);
    ll_append(collision_list, list_node_value);

    ht->size++;

    return 0;
}

extern node_value_t* ht_get(hashtable_t *ht, node_value_t *key)
{
    /* FIXME: stub */
    return NULL;
}

extern node_value_t* ht_remove(hashtable_t *ht, node_value_t *key)
{
    /* FIXME: stub */
    return NULL;
}

extern size_t ht_get_capacity(hashtable_t *ht)
{
    return ht->capacity;
}

extern size_t ht_get_size(hashtable_t *ht)
{
    return ht->size;
}
