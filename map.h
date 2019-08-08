/*
 * map.h
 *
 * Common data types for map-style containers.
 */

#ifndef CCOLL_MAP_H
#define CCOLL_MAP_H

typedef enum {
    NONE, INVALID_KEY
} ccoll_map_error;

typedef enum {
    INSERTION_FAILED, ADDED, REPLACED
} ccoll_map_insertion_status;

typedef enum {
    REMOVAL_FAILED, NOT_FOUND, REMOVED
} ccoll_map_removal_status;

typedef struct ccoll_map_insertion_result {
    ccoll_map_insertion_status status;
    ccoll_map_error error;
    void *old_key;
    void *old_value;
} ccoll_map_insertion_result_t;

typedef struct ccoll_map_removal_result {
    ccoll_map_removal_status status;
    ccoll_map_error error;
    void *key;
    void *value;
} ccoll_map_removal_result_t;

#endif /* CCOLL_MAP_H */
