#include <stdio.h>
#include <stdlib.h>
#include "hashtable.h"
#include "hashtable_test.h"

int ht_test_main() {
    hashtable_t *ht = ht_init();
    return EXIT_SUCCESS;
}

void print_hashtable(hashtable_t *ht) {
    for (size_t i=0; i<ht->capacity; i++) {
        linkedlist_t *collision_list = ht->hash_slots[i];
        ll_iter_t *iter = ll_get_iter(collision_list);

        printf("%lu -> \t", i);
        while (ll_has_next(iter)) {
            ll_node_t *node = ll_next(iter);
            ht_key_value_pair_t *pair = (ht_key_value_pair_t*) node->value;
            printf("%p: %p\n", (void*) pair->key, pair->value);
        }
    }
}
