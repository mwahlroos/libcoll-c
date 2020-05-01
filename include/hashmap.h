/*
 * hashmap.h
 *
 * a basic implementation of a hashmap
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

#include "linkedlist.h"
#include "map.h"
#include "types.h"

#ifndef LIBCOLL_HASHMAP_H
#define LIBCOLL_HASHMAP_H

#define LIBCOLL_HASHMAP_DEFAULT_INIT_SIZE         32
#define LIBCOLL_HASHMAP_DEFAULT_MAX_LOAD_FACTOR   0.75f

typedef struct libcoll_hashmap_entry {
    const void *key;
    const void *value;
} libcoll_hashmap_entry_t;

typedef struct libcoll_hashmap {
    libcoll_linkedlist_t **buckets;
    size_t capacity;
    size_t total_entries;
    float max_load_factor;
    unsigned long (*hash_code_function)(const void *key);
    int (*key_comparator_function)(const void *key1, const void *key2);
    int (*value_comparator_function)(const void *value1, const void *value2);
} libcoll_hashmap_t;

typedef struct libcoll_hashmap_iter {
    libcoll_hashmap_t *hm;
    size_t bucket_index;
    libcoll_linkedlist_node_t *list_node;
} libcoll_hashmap_iter_t;

libcoll_hashmap_t* libcoll_hashmap_init();

libcoll_hashmap_t* libcoll_hashmap_init_with_params(
        size_t init_capacity,
        float max_load_factor,
        unsigned long (*hash_code_function)(const void*),
        int (*key_comparator_function)(const void *key1, const void *key2),
        int (*value_comparator_function)(const void *value1, const void *value2));

void libcoll_hashmap_deinit(libcoll_hashmap_t *hm);

libcoll_map_insertion_result_t libcoll_hashmap_put(libcoll_hashmap_t *hm, const void *key, const void *value);

void* libcoll_hashmap_get(const libcoll_hashmap_t *hm, const void *key);

char libcoll_hashmap_contains(const libcoll_hashmap_t *hm, const void *key);

libcoll_map_removal_result_t libcoll_hashmap_remove(libcoll_hashmap_t *hm, const void *key);

size_t libcoll_hashmap_get_capacity(const libcoll_hashmap_t *hm);

size_t libcoll_hashmap_get_size(const libcoll_hashmap_t *hm);

char libcoll_hashmap_is_empty(const libcoll_hashmap_t *hm);

libcoll_hashmap_iter_t *libcoll_hashmap_get_iterator(libcoll_hashmap_t *hm);

void libcoll_hashmap_drop_iterator(libcoll_hashmap_iter_t *iter);

char libcoll_hashmap_iter_has_next(libcoll_hashmap_iter_t *iter);

libcoll_hashmap_entry_t* libcoll_hashmap_iter_next(libcoll_hashmap_iter_t *iter);

char libcoll_hashmap_iter_has_previous(libcoll_hashmap_iter_t *iter);

libcoll_hashmap_entry_t* libcoll_hashmap_iter_previous(libcoll_hashmap_iter_t *iter);

#endif  /* LIBCOLL_HASHMAP_H */
