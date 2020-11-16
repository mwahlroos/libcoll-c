/*
 * hashmap.c
 *
 * This file is part of libcoll, a generic collections library for C.
 *
 * Copyright (c) 2010-2020 Mika Wahlroos (mika.wahlroos@iki.fi)
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <stdlib.h>
#include <sys/types.h>  /* for ssize_t */

#include "comparators.h"
#include "hash.h"
#include "hashmap.h"
#include "map.h"

#include "debug.h"

static size_t hash(const libcoll_hashmap_t *hm, const size_t hashcode)
{
    /* trivial distribution for now */
    return hashcode % hm->capacity;
}

static libcoll_linkedlist_t* find_collision_list(const libcoll_hashmap_t *hm, const void *key)
{
    size_t slot_index = hash(hm, hm->hash_code_function(key));
    DEBUGF("find_collision_list: hashed to bucket %lu\n", slot_index);
    libcoll_linkedlist_t *collision_list = hm->buckets[slot_index];

    return collision_list;
}

static libcoll_hashmap_entry_t* find_entry(const libcoll_hashmap_t *hm, const void *key)
{
    libcoll_hashmap_entry_t *matching_entry = NULL;
    libcoll_linkedlist_t *collision_list = find_collision_list(hm, key);

    if (NULL != collision_list) {
        DEBUGF("find_entry: found collision list for key %p\n", key);
        libcoll_linkedlist_iter_t *iter = libcoll_linkedlist_get_iter(collision_list);

        while (libcoll_linkedlist_iter_has_next(iter)) {
            libcoll_hashmap_entry_t *entry_tmp;
            const void *key_tmp;

            entry_tmp = libcoll_linkedlist_iter_next(iter);
            key_tmp = entry_tmp->key;

            if (hm->key_comparator_function(key, key_tmp) == 0) {
                matching_entry = entry_tmp;
                break;
            }
        }
        libcoll_linkedlist_drop_iter(iter);
    } else {
        DEBUGF("find_entry: no collision list found for key %p\n", key);
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
static libcoll_map_insertion_result_t insert_new(libcoll_hashmap_t *hm, const void *key, const void *value)
{
    libcoll_map_insertion_result_t result;

    if (NULL == key) {
        result.status = MAP_INSERTION_FAILED;
        result.error = MAP_ERROR_INVALID_KEY;
        return result;
    }

    size_t slot_index = hash(hm, hm->hash_code_function(key));
    DEBUGF("insert_new: inserting at bucket %lu\n", slot_index);
    libcoll_linkedlist_t *collision_list = hm->buckets[slot_index];

    if (NULL == collision_list) {
        collision_list = libcoll_linkedlist_init_with_comparator(hm->key_comparator_function);
        hm->buckets[slot_index] = collision_list;
    }

    libcoll_linkedlist_iter_t *iter = libcoll_linkedlist_get_iter(collision_list);
    libcoll_hashmap_entry_t *entry;
    char key_exists = 0;

    while ((entry = (libcoll_hashmap_entry_t*) libcoll_linkedlist_iter_next(iter)) != NULL) {
        if (hm->key_comparator_function(key, entry->key) == 0) {
            DEBUG("insert_new: replacing existing entry with matching key\n");
            result.old_key = (void*) entry->key;
            result.old_value = (void*) entry->value;
            entry->key = key;
            entry->value = value;

            result.status = MAP_ENTRY_REPLACED;
            result.error = MAP_ERROR_NONE;
            key_exists = 1;
            break;
        }
    }
    libcoll_linkedlist_drop_iter(iter);

    if (!key_exists) {
        libcoll_hashmap_entry_t *new_entry = malloc(sizeof(libcoll_hashmap_entry_t));
        new_entry->key = key;
        new_entry->value = value;

        libcoll_linkedlist_append(collision_list, new_entry);
        result.status = MAP_ENTRY_ADDED;
        result.error = MAP_ERROR_NONE;
    }

    return result;
}

static void resize(libcoll_hashmap_t *hm, size_t capacity)
{
    size_t old_cap = hm->capacity;
    libcoll_linkedlist_t **old_buckets = hm->buckets;
    libcoll_linkedlist_t **new_buckets = calloc(capacity, sizeof(libcoll_linkedlist_t*));
    hm->buckets = new_buckets;
    hm->capacity = capacity;

    DEBUG("\n");
    for (size_t i=0; i<old_cap; i++) {
        DEBUGF("resize: rehashing elements in bucket %lu\n", i);
        libcoll_linkedlist_t *list = old_buckets[i];
        if (NULL != list) {
            DEBUG("resize: nonempty list\n");
            libcoll_linkedlist_iter_t *iter = libcoll_linkedlist_get_iter(list);
            libcoll_hashmap_entry_t *old_entry;
            while ((old_entry = libcoll_linkedlist_iter_next(iter)) != NULL) {
                DEBUG("resize: reallocating list entry\n");
                /* TODO: There shouldn't be any need to allocate all-new entries
                 * and freeing the old ones. Could be improved, requires a bit
                 * of reorganization.
                 */
                libcoll_map_insertion_result_t res = insert_new(hm, old_entry->key, old_entry->value);
                DEBUGF("resize: insertion result: status=%d, error=%d\n", res.status, res.error);

                free(old_entry);
            }
            libcoll_linkedlist_drop_iter(iter);
            libcoll_linkedlist_deinit(list);
        }
    }
    free(old_buckets);
}

static ssize_t find_next_nonempty_bucket(libcoll_hashmap_t *hm, size_t start_index)
{
    while (start_index < hm->capacity) {
        if (hm->buckets[start_index] != NULL) {
            return start_index;
        }

        start_index++;
    }

    return -1;
}

static ssize_t find_previous_nonempty_bucket(libcoll_hashmap_t *hm, size_t start_index)
{
    ssize_t tmpindex = (ssize_t) start_index;

    while (tmpindex >= 0) {
        if (hm->buckets[tmpindex] != NULL) {
            return tmpindex;
        }

        tmpindex--;
    }

    return -1;
}

libcoll_hashmap_t* libcoll_hashmap_init()
{
    return libcoll_hashmap_init_with_params(
        LIBCOLL_HASHMAP_DEFAULT_INIT_SIZE,
        LIBCOLL_HASHMAP_DEFAULT_MAX_LOAD_FACTOR,
        NULL, NULL, NULL);
}

libcoll_hashmap_t* libcoll_hashmap_init_with_params(
        size_t init_capacity,
        float max_load_factor,
        unsigned long (*hash_code_function)(const void* key),
        int (*key_comparator_function)(const void *key1, const void *key2),
        int (*value_comparator_function)(const void *value1, const void *value2))
{
    libcoll_hashmap_t *hm = malloc(sizeof(libcoll_hashmap_t));

    /* calloc automatically sets the entire allocated memory to zeros/NULLs,
     * which is useful in this case since it means unused buckets are
     * guaranteed to contain NULLs
     */
    libcoll_linkedlist_t **buckets = (libcoll_linkedlist_t**) calloc(init_capacity, sizeof(libcoll_linkedlist_t*));

    hm->buckets = buckets;
    hm->max_load_factor = max_load_factor;
    hm->capacity = init_capacity;
    hm->total_entries = 0;

    if (NULL != hash_code_function) {
        hm->hash_code_function = hash_code_function;
    } else {
        hm->hash_code_function = &libcoll_hashcode_memaddr;
    }

    if (NULL != key_comparator_function) {
        hm->key_comparator_function = key_comparator_function;
    } else {
        hm->key_comparator_function = &libcoll_memaddrcmp;
    }

    if (NULL != value_comparator_function) {
        hm->value_comparator_function = value_comparator_function;
    } else {
        hm->value_comparator_function = &libcoll_memaddrcmp;
    }

    return hm;
}

void libcoll_hashmap_deinit(libcoll_hashmap_t *hm)
{
    for (size_t i=0; i<hm->capacity; i++) {
        libcoll_linkedlist_t *list = hm->buckets[i];
        if (NULL != list) {
            DEBUGF("Hashmap deinit: clearing collision list (%lu entries) at bucket %lu\n",
                   libcoll_linkedlist_length(list), i);

            libcoll_linkedlist_iter_t *iter = libcoll_linkedlist_get_iter(list);
            while (libcoll_linkedlist_iter_has_next(iter)) {
                DEBUG("Hashmap deinit: freeing entry\n");

                libcoll_hashmap_entry_t *entry =
                    (libcoll_hashmap_entry_t*) libcoll_linkedlist_iter_next(iter);

                free(entry);
            }
            libcoll_linkedlist_drop_iter(iter);
            libcoll_linkedlist_deinit(list);
        }
    }
    free(hm->buckets);
    free(hm);
}

libcoll_map_insertion_result_t libcoll_hashmap_put(libcoll_hashmap_t *hm, const void *key, const void *value)
{
    libcoll_map_insertion_result_t result = insert_new(hm, key, value);
    if (result.status == MAP_ENTRY_ADDED) {
        hm->total_entries++;
        float load = (float) hm->total_entries / hm->capacity;
        if (load > hm->max_load_factor) {
            resize(hm, hm->capacity * 2);
        }
    }

    return result;
}

void* libcoll_hashmap_get(const libcoll_hashmap_t *hm, const void *key)
{
    libcoll_hashmap_entry_t *entry = find_entry(hm, key);

    if (NULL != entry) {
        return (void*) entry->value;
    } else {
        return NULL;
    }
}

char libcoll_hashmap_contains(const libcoll_hashmap_t *hm, const void *key)
{
    libcoll_hashmap_entry_t *entry = find_entry(hm, key);
    return NULL != entry;
}

libcoll_map_removal_result_t libcoll_hashmap_remove(libcoll_hashmap_t *hm, const void *key)
{
    libcoll_map_removal_result_t result;

    if (NULL == key) {
        result.status = MAP_REMOVAL_FAILED;
        result.error = MAP_ERROR_INVALID_KEY;
        return result;
    }

    result.status = KEY_NOT_FOUND;
    result.key = NULL;
    result.value = NULL;

    unsigned long key_hash = hash(hm, hm->hash_code_function(key));
    size_t slot_index = (key_hash % hm->capacity);
    libcoll_linkedlist_t *collision_list = hm->buckets[slot_index];

    if (NULL != collision_list) {
        libcoll_linkedlist_iter_t *iter = libcoll_linkedlist_get_iter(collision_list);
        while (libcoll_linkedlist_iter_has_next(iter)) {
            libcoll_hashmap_entry_t *entry = (libcoll_hashmap_entry_t*) libcoll_linkedlist_iter_next(iter);
            if (hm->key_comparator_function(key, entry->key) == 0) {
                result.key = (void*) entry->key;
                result.value = (void*) entry->value;
                result.status = MAP_ENTRY_REMOVED;
                libcoll_linkedlist_iter_remove(iter);
                hm->total_entries--;
                free(entry);
            }
        }
        libcoll_linkedlist_drop_iter(iter);
    }

    return result;
}

size_t libcoll_hashmap_get_capacity(const libcoll_hashmap_t *hm)
{
    return hm->capacity;
}

size_t libcoll_hashmap_get_size(const libcoll_hashmap_t *hm)
{
    return hm->total_entries;
}

char libcoll_hashmap_is_empty(const libcoll_hashmap_t *hm)
{
    return hm->total_entries == 0;
}

libcoll_hashmap_iter_t* libcoll_hashmap_get_iterator(libcoll_hashmap_t *hm)
{
    libcoll_hashmap_iter_t *iter = malloc(sizeof(libcoll_hashmap_iter_t));
    iter->hm = hm;
    iter->bucket_index = 0;
    iter->list_node = NULL;

    return iter;
}

void libcoll_hashmap_drop_iterator(libcoll_hashmap_iter_t *iter)
{
    free(iter);
}

char libcoll_hashmap_iter_has_next(libcoll_hashmap_iter_t *iter)
{
    if (iter->list_node != NULL && iter->list_node->next != NULL) {
        return 1;
    } else {
        return find_next_nonempty_bucket(iter->hm, iter->bucket_index) != -1;
    }
}

libcoll_hashmap_entry_t* libcoll_hashmap_iter_next(libcoll_hashmap_iter_t *iter)
{
    libcoll_linkedlist_node_t *next = NULL;

    if (iter->list_node != NULL && iter->list_node->next != NULL) {
        next = iter->list_node->next;
    } else {
        ssize_t next_nonempty_bucket = find_next_nonempty_bucket(iter->hm, iter->bucket_index);
        if (next_nonempty_bucket != -1) {
            libcoll_linkedlist_t *list = iter->hm->buckets[next_nonempty_bucket];
            next = list->head;
            iter->bucket_index = next_nonempty_bucket + 1;
        }
    }

    iter->list_node = next;

    return next != NULL ? (libcoll_hashmap_entry_t*) next->value : NULL;
}

char libcoll_hashmap_iter_has_previous(libcoll_hashmap_iter_t *iter)
{
    if (iter->list_node != NULL && iter->list_node->previous != NULL) {
        return 1;
    } else {
        return find_previous_nonempty_bucket(iter->hm, iter->bucket_index) != -1;
    }
}

libcoll_hashmap_entry_t* libcoll_hashmap_iter_previous(libcoll_hashmap_iter_t *iter)
{
    libcoll_linkedlist_node_t *previous = NULL;

    if (iter->list_node != NULL && iter->list_node->previous != NULL) {
        previous = iter->list_node->previous;
    } else {
        ssize_t previous_nonempty_bucket = find_previous_nonempty_bucket(iter->hm, iter->bucket_index);
        if (previous_nonempty_bucket != -1) {
            libcoll_linkedlist_t *list = iter->hm->buckets[previous_nonempty_bucket];
            previous = list->tail;
            iter->bucket_index = previous_nonempty_bucket - 1;
        }
    }

    iter->list_node = previous;

    return previous != NULL ? (libcoll_hashmap_entry_t*) previous->value : NULL;
}
