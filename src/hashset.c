/*
 * hashset.c
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

#include <stdlib.h>

#include "hashmap.h"
#include "hashset.h"
#include "set.h"

#include "debug.h"

libcoll_hashset_t* libcoll_hashset_init()
{
    return libcoll_hashset_init_with_params(
        LIBCOLL_HASHMAP_DEFAULT_INIT_SIZE,
        LIBCOLL_HASHMAP_DEFAULT_MAX_LOAD_FACTOR,
        NULL, NULL);
}

libcoll_hashset_t* libcoll_hashset_init_with_params(
        size_t init_capacity,
        float max_load_factor,
        unsigned long (*hash_code_function)(const void*),
        int (*key_comparator_function)(const void *key1, const void *key2))
{
    libcoll_hashset_t *hashset = malloc(sizeof(libcoll_hashset_t));

    libcoll_hashmap_t *backing_hashmap =
        libcoll_hashmap_init_with_params(init_capacity,
                                         max_load_factor,
                                         hash_code_function,
                                         key_comparator_function,
                                         NULL);

    hashset->backing_hashmap = backing_hashmap;

    return hashset;
}

void libcoll_hashset_deinit(libcoll_hashset_t *hs)
{
    libcoll_hashmap_deinit(hs->backing_hashmap);
    free(hs);
}

libcoll_set_insertion_result_t libcoll_hashset_put(libcoll_hashset_t *hs, const void *key)
{
    libcoll_map_insertion_result_t tmp_res =
        libcoll_hashmap_put(hs->backing_hashmap, key, NULL);

    /* wrap status and error information from the map API */

    libcoll_set_insertion_result_t res;

    res.old_key = tmp_res.old_key;

    switch (tmp_res.status) {
        case MAP_INSERTION_FAILED:
            res.status = SET_INSERTION_FAILED;
            break;
        case MAP_ENTRY_ADDED:
            res.status = SET_ENTRY_ADDED;
            break;
        case MAP_ENTRY_REPLACED:
            res.status = SET_ENTRY_REPLACED;
            break;
        default:
            DEBUGF("Unknown map insertion status: %d\n", tmp_res.status);
    }

    switch (tmp_res.error) {
        case MAP_ERROR_NONE:
            res.error = SET_ERROR_NONE;
            break;
        case MAP_ERROR_INVALID_KEY:
            res.error = SET_ERROR_INVALID_KEY;
            break;
        default:
            DEBUGF("Unknown map insertion error: %d\n", tmp_res.error);
    }

    return res;
}

void* libcoll_hashset_get(const libcoll_hashset_t *hs, const void *key)
{
    libcoll_hashmap_entry_t *map_entry = _find_entry(hs->backing_hashmap, key);

    if (NULL != map_entry) {
        return (void*) map_entry->key;
    } else {
        return NULL;
    }
}

char libcoll_hashset_contains(const libcoll_hashset_t *hs, const void *key)
{
    libcoll_hashmap_entry_t *map_entry = _find_entry(hs->backing_hashmap, key);
    return NULL != map_entry;
}

libcoll_set_removal_result_t libcoll_hashset_remove(libcoll_hashset_t *hs, const void *key)
{
    libcoll_map_removal_result_t tmp_res = libcoll_hashmap_remove(hs->backing_hashmap, key);

    /* wrap status and error information from the map API */

    libcoll_set_removal_result_t res;

    res.key = tmp_res.key;

    switch (tmp_res.status) {
        case MAP_REMOVAL_FAILED:
            res.status = SET_REMOVAL_FAILED;
            break;
        case MAP_KEY_NOT_FOUND:
            res.status = SET_KEY_NOT_FOUND;
            break;
        case MAP_ENTRY_REMOVED:
            res.status = SET_ENTRY_REMOVED;
            break;
        default:
            DEBUGF("Unknown map removal status: %d\n", tmp_res.status);
    }

    switch (tmp_res.error) {
        case MAP_ERROR_NONE:
            res.error = SET_ERROR_NONE;
            break;
        case MAP_ERROR_INVALID_KEY:
            res.error = SET_ERROR_INVALID_KEY;
            break;
        default:
            DEBUGF("Unknown map removal error: %d\n", tmp_res.error);
    }

    return res;
}

size_t libcoll_hashset_get_capacity(const libcoll_hashset_t *hs)
{
    return hs->backing_hashmap->capacity;
}

size_t libcoll_hashset_get_size(const libcoll_hashset_t *hs)
{
    return hs->backing_hashmap->total_entries;
}

char libcoll_hashset_is_empty(const libcoll_hashset_t *hs)
{
    return hs->backing_hashmap->total_entries == 0;
}
