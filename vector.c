/*
 * vector.c
 */

#include <stdlib.h>
#include "node.h"
#include "vector.h"

/* declarations of internal functions */
static void resize_if_full(libcoll_vector_t *vector);

/* public API functions */

libcoll_vector_t* libcoll_vector_init()
{
    return libcoll_vector_init_with_params(
        LIBCOLL_VECTOR_DEFAULT_INIT_CAPACITY,
        _libcoll_node_comparator_memaddr
    );
}

libcoll_vector_t* libcoll_vector_init_with_params(
    size_t initial_capacity,
    int (*compare_function)(void *value1, void *value2))
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
    /* TODO: check for errors in reallocation */
    for (size_t i=vector->length-1; i>=index; i--) {
        vector->contents[i+1] = vector->contents[i];
    }
    vector->contents[index] = value;
}

void* libcoll_vector_remove(libcoll_vector_t *vector, size_t index)
{
    if (index >= vector->length) return NULL;

    void *value = vector->contents[index];
    for (size_t i=index; i<vector->length; i++) {
        vector->contents[i] = vector->contents[i+1];
    }
    vector->length--;
    return value;
}

/* internal functions */

static void resize_if_full(libcoll_vector_t *vector)
{
    if (vector->length == vector->capacity) {
        /* TODO: check for realloc errors */
        size_t new_cap = 2 * vector->capacity;
        vector->contents = realloc(vector->contents, new_cap);
        vector->capacity = new_cap;
    }
}
