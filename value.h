/*
 * value.h
 */

#include <stdlib.h>

#ifndef VALUE_H
#define VALUE_H
#endif

typedef struct value {
    size_t size;
    void *payload;
} value_t;
