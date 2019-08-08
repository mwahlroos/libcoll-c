/*
 * hash.h
 *
 * Functions for computing hash codes for common types.
 */

#include <string.h>
#include "hash.h"

unsigned long hashcode_int(void *intptr)
{
    int val = *(int*) intptr;
    return (unsigned long) val;
}

unsigned long hashcode_str(void *str)
{
    char *s = (char*) str;
    unsigned long hash = 31;
    for (size_t i=0; i<strlen(s); i++) {
        hash = 37 * hash + s[i];
    }
    return hash;
}

unsigned long hashcode_str2(void *str)
{
    /* use the djb2 algorithm for computing a hash code for a string;
     * shamelessly copied from http://www.cse.yorku.ca/~oz/hash.html
     */

    char *s = (char*) str;

    unsigned long hash = 5381;
    int c;
    while (c = *s++) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}
