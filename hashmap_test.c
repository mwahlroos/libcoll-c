#include <stdio.h>
#include <stdlib.h>
#include "hashmap.h"
#include "hashmap_test.h"

int hm_test_main() {
    hashmap_t *hm = hm_init();
    return EXIT_SUCCESS;
}

void print_hashmap(hashmap_t *hm) {
    for (size_t i=0; i<hm->capacity; i++) {
        linkedlist_t *collision_list = hm->hash_slots[i];
        ll_iter_t *iter = ll_get_iter(collision_list);

        printf("%lu -> \t", i);
        while (ll_iter_has_next(iter)) {
            ll_node_t *node = ll_iter_next(iter);
            hm_entry_t *pair = (hm_entry_t*) node->value;
            printf("%p: %p\n", (void*) pair->key, pair->value);
        }
    }
}
