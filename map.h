/*
 * map.h
 *
 * Common data types for map-style containers.
 */

#ifndef LIBCOLL_MAP_H
#define LIBCOLL_MAP_H

typedef enum {
    NONE, INVALID_KEY
} libcoll_map_error;

typedef enum {
    INSERTION_FAILED, ADDED, REPLACED
} libcoll_map_insertion_status;

typedef enum {
    REMOVAL_FAILED, NOT_FOUND, REMOVED
} libcoll_map_removal_status;

typedef struct libcoll_map_insertion_result {
    libcoll_map_insertion_status status;
    libcoll_map_error error;
    void *old_key;
    void *old_value;
} libcoll_map_insertion_result_t;

typedef struct libcoll_map_removal_result {
    libcoll_map_removal_status status;
    libcoll_map_error error;
    void *key;
    void *value;
} libcoll_map_removal_result_t;

#endif /* LIBCOLL_MAP_H */
