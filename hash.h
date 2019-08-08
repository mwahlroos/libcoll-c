/*
 * hash.h
 *
 * Functions for computing hash codes for common types.
 */

#ifndef CCOLL_HASH_H
#define CCOLL_HASH_H

unsigned long hashcode_int(void *intptr);

unsigned long hashcode_str(void *str);

unsigned long hashcode_str2(void *str);

#endif /* CCOLL_HASH_H */
