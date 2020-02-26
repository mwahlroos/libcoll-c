/*
 * list.h
 *
 * Common data types for list-style containers.
 */

#ifndef LIBCOLL_LIST_H
#define LIBCOLL_LIST_H

typedef enum {
    LIST_ERROR_NONE, LIST_INDEX_OUT_OF_RANGE
} libcoll_list_error;

typedef enum {
    LIST_ADDITION_FAILED, LIST_ENTRY_ADDED
} libcoll_list_addition_status;

typedef enum {
    LIST_REPLACEMENT_FAILED, LIST_ITEM_REPLACED
} libcoll_list_replacement_status;

typedef enum {
    LIST_REMOVAL_FAILED, LIST_KEY_NOT_FOUND, LIST_ENTRY_REMOVED
} libcoll_list_removal_status;

typedef struct libcoll_list_addition_result {
    libcoll_list_addition_status status;
    libcoll_list_error error;
} libcoll_list_addition_result_t;

typedef struct libcoll_list_replacement_result {
    libcoll_list_replacement_status status;
    libcoll_list_error error;
    void *old_value;
} libcoll_list_replacement_result_t;

typedef struct libcoll_list_removal_result {
    libcoll_list_removal_status status;
    libcoll_list_error error;
    void *value;
} libcoll_list_removal_result_t;

#endif /* LIBCOLL_LIST_H */
