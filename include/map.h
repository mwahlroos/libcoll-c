/*
 * map.h
 *
 * Common data types for map-style containers.
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
