/*
 * vector.c
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
#include "vector.h"

#include "debug.h"

/* declarations of internal functions */
static void resize_if_full(libcoll_vector_t *vector);

/* public API functions */

libcoll_vector_t* libcoll_vector_init()
{
    return libcoll_vector_init_with_params(
        LIBCOLL_VECTOR_DEFAULT_INIT_CAPACITY,
        libcoll_memaddrcmp
    );
}

libcoll_vector_t* libcoll_vector_init_with_params(
    size_t initial_capacity,
    int (*compare_function)(const void *value1, const void *value2))
{
    libcoll_vector_t *vector = malloc(sizeof(libcoll_vector_t));
    vector->capacity = initial_capacity;
    vector->length = 0;
    vector->compare_function = compare_function;
    vector->contents = malloc(initial_capacity * sizeof(void*));
    return vector;
}

void libcoll_vector_deinit(libcoll_vector_t *vector)
{
    free(vector->contents);
    free(vector);
}

void libcoll_vector_append(libcoll_vector_t *vector, void *value)
{
    resize_if_full(vector);
    vector->contents[(vector->length)++] = value;
}

void libcoll_vector_insert(libcoll_vector_t *vector, size_t index, void *value)
{
    resize_if_full(vector);
    if (vector->length > 0) {
        for (size_t i=vector->length; i>index; i--) {
            vector->contents[i] = vector->contents[i-1];
        }
    }
    vector->contents[index] = value;
    vector->length++;
}

void* libcoll_vector_remove(libcoll_vector_t *vector, void *value)
{
    void *retvalue = NULL;
    ssize_t index = libcoll_vector_index_of(vector, value);
    if (index != -1) {
        retvalue = libcoll_vector_remove_at(vector, (size_t) index);
    }

    return retvalue;
}

void* libcoll_vector_remove_at(libcoll_vector_t *vector, size_t index)
{
    if (index >= vector->length) return NULL;

    void *value = vector->contents[index];
    for (size_t i=index; i<vector->length; i++) {
        vector->contents[i] = vector->contents[i+1];
    }
    vector->length--;
    return value;
}

void* libcoll_vector_get(libcoll_vector_t *vector, size_t index)
{
    return index < vector->length ? vector->contents[index] : NULL;
}

void* libcoll_vector_pop(libcoll_vector_t *vector)
{
    return libcoll_vector_remove_at(vector, vector->length-1);
}

ssize_t libcoll_vector_index_of(libcoll_vector_t *vector, void *value)
{
    for (size_t i=0; i<vector->length; i++) {
        if (!vector->compare_function(value, vector->contents[i])) {
            return i;
        }
    }
    return -1;
}

ssize_t libcoll_vector_last_index_of(libcoll_vector_t *vector, void *value)
{
    for (size_t i=vector->length; i>0; i--) {
        if (!vector->compare_function(value, vector->contents[i-1])) {
            return i-1;
        }
    }
    return -1;
}

char libcoll_vector_contains(libcoll_vector_t *vector, void *value)
{
    return libcoll_vector_index_of(vector, value) != -1;
}

size_t libcoll_vector_length(libcoll_vector_t *vector)
{
    return vector->length;
}

char libcoll_vector_is_empty(libcoll_vector_t *vector)
{
    return vector->length == 0;
}

libcoll_vector_iter_t *libcoll_vector_get_iter(libcoll_vector_t *vector)
{
    libcoll_vector_iter_t *iter = malloc(sizeof(libcoll_vector_iter_t));
    iter->vector = vector;
    iter->next_index = 0;
    iter->last_skip_forward = 0;

    return iter;
}

void libcoll_vector_drop_iter(libcoll_vector_iter_t *iter)
{
    free(iter);
}

char libcoll_vector_iter_has_next(libcoll_vector_iter_t *iter)
{
    return iter->vector->length > iter->next_index;
}

char libcoll_vector_iter_has_previous(libcoll_vector_iter_t *iter)
{
    return iter->next_index != 0;
}

void* libcoll_vector_iter_next(libcoll_vector_iter_t *iter)
{
    if (iter->next_index >= iter->vector->length) {
        return NULL;
    }

    void *value = iter->vector->contents[iter->next_index];
    iter->next_index++;
    iter->last_skip_forward = 1;
    return value;
}

void* libcoll_vector_iter_previous(libcoll_vector_iter_t *iter)
{
    if (iter->next_index == 0) {
        return NULL;
    }

    void *value = iter->vector->contents[iter->next_index-1];
    iter->next_index--;
    iter->last_skip_forward = 0;
    return value;
}

void libcoll_vector_iter_remove(libcoll_vector_iter_t *iter)
{
    void *value;
    if (iter->last_skip_forward) {
        value = libcoll_vector_remove_at(iter->vector, iter->next_index-1);
        if (value != NULL) {
            iter->next_index--;
        } else {
            ERRORF("libcoll_vector_iter_remove: removal failed; index %lu out of vector range\n", iter->next_index-1);
        }
    } else {
        value = libcoll_vector_remove_at(iter->vector, iter->next_index);
        if (value == NULL) {
            ERRORF("libcoll_vector_iter_remove: removal failed; index %lu out of vector range\n", iter->next_index-1);
        }
    }
}

/* internal functions */

static void resize_if_full(libcoll_vector_t *vector)
{
    if (vector->length == vector->capacity) {
        /* TODO: check for realloc errors */
        size_t new_cap;
        if (vector->capacity > 0) {
            new_cap = 2 * vector->capacity;
        } else {
            new_cap = 1;
        }
        vector->contents = realloc(vector->contents, new_cap * sizeof(void*));
        vector->capacity = new_cap;
    }
}
