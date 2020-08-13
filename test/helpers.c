/*
 * Unit tests for the libcoll library.
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

#include <stdio.h>
#include <string.h>
#include "hashmap.h"

#include "../src/debug.h"

/* helper functions for tests */

static void print_hashmap_contents(const libcoll_hashmap_t *hm, FILE *out);


void print_hashmap(const libcoll_hashmap_t *hm, FILE *out)
{
    fprintf(out, "Hashmap contents/capacity: %lu/%lu\n",
          libcoll_hashmap_get_size(hm),
          libcoll_hashmap_get_capacity(hm)
    );
    fprintf(out, "Hashmap contents:\n");
    print_hashmap_contents(hm, out);
}

static void print_hashmap_contents(const libcoll_hashmap_t *hm, FILE *out)
{
    for (size_t i=0; i<hm->capacity; i++) {
        libcoll_linkedlist_t *collision_list = hm->buckets[i];
        if (NULL == collision_list) {
            fprintf(out, "[%lu] empty bucket\n", i);
        } else {
            fprintf(out, "[%lu] nonempty bucket with %lu entries:", i, libcoll_linkedlist_length(collision_list));
            libcoll_linkedlist_iter_t *iter = libcoll_linkedlist_get_iter(collision_list);
            while (libcoll_linkedlist_iter_has_next(iter)) {
                libcoll_hashmap_entry_t *entry =
                    (libcoll_hashmap_entry_t*) libcoll_linkedlist_iter_next(iter);

                fprintf(out, " (%p -> %p)", entry->key, entry->value);
            }
            fprintf(out, "\n");

            libcoll_linkedlist_drop_iter(iter);
        }
    }
}
