/*
 * vector.h
 */

#include <stdlib.h>

#ifndef LIBCOLL_VECTOR_H
#define LIBCOLL_VECTOR_H

#define LIBCOLL_VECTOR_DEFAULT_INIT_CAPACITY    10

typedef struct libcoll_vector
{
    void **contents;
    size_t length;    /* the number of elements in the vector */
    size_t capacity;  /* the length of the memory currently allocated */
    int (*compare_function)(void *value1, void *value2);
} libcoll_vector_t;

libcoll_vector_t* libcoll_vector_init();

libcoll_vector_t* libcoll_vector_init_with_params(
    size_t initial_capacity,
    int (*compare_function)(void *value1, void *value2)
);

void libcoll_vector_deinit(libcoll_vector_t *vector);

void libcoll_vector_append(libcoll_vector_t *vector, void *value);

void libcoll_vector_insert(libcoll_vector_t *vector, size_t index, void *value);

void* libcoll_vector_remove(libcoll_vector_t *vector, size_t index);

#endif /* LIBCOLL_VECTOR_H */
