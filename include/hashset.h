/*
 * hashset.h
 *
 * a basic implementation of a hashset
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

#include "hashmap.h"
#include "set.h"

#ifndef LIBCOLL_HASHSET_H
#define LIBCOLL_HASHSET_H

/*
 * A type definition for hashsets.
 *
 * Hashsets are implemented as convenience wrappers around the hashmap type,
 * with an interface that basically deals with only keys rather than key-value
 * pairs.
 */
typedef struct libcoll_hashset {
    libcoll_hashmap_t *backing_hashmap;
} libcoll_hashset_t;

libcoll_hashset_t* libcoll_hashset_init();

libcoll_hashset_t* libcoll_hashset_init_with_params(
        size_t init_capacity,
        float max_load_factor,
        unsigned long (*hash_code_function)(const void*),
        int (*key_comparator_function)(const void *key1, const void *key2));

void libcoll_hashset_deinit(libcoll_hashset_t *hs);

libcoll_set_insertion_result_t libcoll_hashset_put(libcoll_hashset_t *hs, const void *key);

void* libcoll_hashset_get(const libcoll_hashset_t *hs, const void *key);

char libcoll_hashset_contains(const libcoll_hashset_t *hs, const void *key);

libcoll_set_removal_result_t libcoll_hashset_remove(libcoll_hashset_t *hs, const void *key);

size_t libcoll_hashset_get_capacity(const libcoll_hashset_t *hs);

size_t libcoll_hashset_get_size(const libcoll_hashset_t *hs);

char libcoll_hashset_is_empty(const libcoll_hashset_t *hs);

#endif  /* LIBCOLL_HASHSET_H */
