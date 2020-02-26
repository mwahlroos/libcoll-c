/*
 * map.h
 *
 * Common data types for map-style containers.
 */

#ifndef LIBCOLL_MAP_H
#define LIBCOLL_MAP_H

typedef enum {
    MAP_ERROR_NONE, MAP_ERROR_INVALID_KEY
} libcoll_map_error;

typedef enum {
    MAP_INSERTION_FAILED, MAP_ENTRY_ADDED, MAP_ENTRY_REPLACED
} libcoll_map_insertion_status;

typedef enum {
    MAP_REMOVAL_FAILED, KEY_NOT_FOUND, MAP_ENTRY_REMOVED
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
