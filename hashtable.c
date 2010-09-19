/*
 * hashtable.c
 */

#include <stdlib.h>
#include "hashtable.h"

extern hashtable_t* ht_init(size_t init_capacity, unsigned int (*hash_function)(ht_key_t*))
{
    hashtable_t *ht = (hashtable_t*) malloc(init_capacity * sizeof(hashtable_t));
    ht->capacity = init_capacity;
    return ht;
}

extern void ht_deinit(hashtable_t *ht)
{
    free(ht);
}

extern int ht_add(hashtable_t *ht, ht_key_t *key, value_t *value)
{
    /* FIXME: stub */
    
    /* unsigned int slot = (ht->hash_function(key) % ht->capacity); */
    
    return 0;
}

extern value_t* ht_get(hashtable_t *ht, ht_key_t *key)
{
    /* FIXME: stub */
    return NULL;
}

extern value_t* ht_remove(hashtable_t *ht, ht_key_t *key)
{
    /* FIXME: stub */
    return NULL;
}

extern size_t ht_get_capacity(hashtable_t *ht)
{
    return ht->capacity;
}

