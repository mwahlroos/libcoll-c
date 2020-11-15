/*
 * vector.h
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

#ifndef LIBCOLL_VECTOR_H
#define LIBCOLL_VECTOR_H

#define LIBCOLL_VECTOR_DEFAULT_INIT_CAPACITY    10

typedef struct libcoll_vector
{
    void **contents;
    size_t length;    /* the number of elements in the vector */
    size_t capacity;  /* the length of the memory currently allocated */
    int (*compare_function)(const void *value1, const void *value2);
} libcoll_vector_t;

typedef struct libcoll_vector_iter
{
    libcoll_vector_t *vector;
    size_t next_index;
    char last_skip_forward;
} libcoll_vector_iter_t;


libcoll_vector_t* libcoll_vector_init();

libcoll_vector_t* libcoll_vector_init_with_params(
    size_t initial_capacity,
    int (*compare_function)(const void *value1, const void *value2)
);

void libcoll_vector_deinit(libcoll_vector_t *vector);

void libcoll_vector_append(libcoll_vector_t *vector, void *value);

void libcoll_vector_insert(libcoll_vector_t *vector, size_t index, void *value);

void* libcoll_vector_remove(libcoll_vector_t *vector, void *value);

void* libcoll_vector_remove_at(libcoll_vector_t *vector, size_t index);

void* libcoll_vector_get(libcoll_vector_t *vector, size_t index);

void* libcoll_vector_pop(libcoll_vector_t *vector);

ssize_t libcoll_vector_index_of(libcoll_vector_t *vector, void *value);

ssize_t libcoll_vector_last_index_of(libcoll_vector_t *vector, void *value);

char libcoll_vector_contains(libcoll_vector_t *vector, void *value);

size_t libcoll_vector_length(libcoll_vector_t *vector);

char libcoll_vector_is_empty(libcoll_vector_t *vector);

libcoll_vector_iter_t *libcoll_vector_get_iter(libcoll_vector_t *vector);

void libcoll_vector_drop_iter(libcoll_vector_iter_t *iter);

char libcoll_vector_iter_has_next(libcoll_vector_iter_t *iter);

char libcoll_vector_iter_has_previous(libcoll_vector_iter_t *iter);

void* libcoll_vector_iter_next(libcoll_vector_iter_t *iter);

void* libcoll_vector_iter_previous(libcoll_vector_iter_t *iter);

void libcoll_vector_iter_remove(libcoll_vector_iter_t *iter);

#endif /* LIBCOLL_VECTOR_H */
