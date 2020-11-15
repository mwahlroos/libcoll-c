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

#include <check.h>

#include "test_vector.h"

#include "comparators.h"
#include "vector.h"

#include "../src/debug.h"

/*
 * Tests that an empty vector gets properly allocated.
 */
START_TEST(vector_create)
{
    DEBUG("\n*** Starting vector_create\n");

    libcoll_vector_t *vector = libcoll_vector_init();

    ck_assert_ptr_nonnull(vector->contents);
    ck_assert(vector->compare_function != NULL);
    libcoll_vector_deinit(vector);
}
END_TEST

/*
 * Tests appending values to a vector and checks that they can be properly
 * retrieved afterwards.
 */
START_TEST(vector_populate_and_retrieve)
{
    DEBUG("\n*** Starting vector_populate_and_retrieve\n");

    libcoll_vector_t *vector =
        libcoll_vector_init_with_params(LIBCOLL_VECTOR_DEFAULT_INIT_CAPACITY, libcoll_strcmp_wrapper);

    char *s1 = "Hello";
    char *s2 = "World";
    char *s3 = "!";

    libcoll_vector_append(vector, s1);
    libcoll_vector_append(vector, s2);
    libcoll_vector_append(vector, s3);
    libcoll_vector_append(vector, s3);

    ck_assert_uint_eq(vector->length, 4);

    ck_assert_int_eq(libcoll_vector_index_of(vector, "Hello"), 0);
    ck_assert_int_eq(libcoll_vector_index_of(vector, "World"), 1);
    ck_assert_int_eq(libcoll_vector_index_of(vector, "!"), 2);
    ck_assert_int_eq(libcoll_vector_last_index_of(vector, "!"), 3);
    ck_assert_int_eq(libcoll_vector_index_of(vector, "nonexistent"), -1);

    ck_assert(strcmp(s1, (char*) vector->contents[0]) == 0);
    ck_assert(strcmp(s2, (char*) vector->contents[1]) == 0);

    ck_assert(libcoll_vector_contains(vector, "Hello"));
    ck_assert(libcoll_vector_contains(vector, "!"));
    ck_assert(!libcoll_vector_contains(vector, "nonexistent"));

    libcoll_vector_deinit(vector);
}
END_TEST

/*
 * Tests that a vector gets properly resized once its capacity is exceeded,
 * and that appended values are correctly retrieved afterwards.
 */
START_TEST(vector_resize)
{
    DEBUG("\n*** Starting vector_resize\n");

    size_t init_capacity = 2LU;
    libcoll_vector_t *vector = libcoll_vector_init_with_params(
        init_capacity,
        libcoll_strcmp_wrapper
    );

    char *s1 = "Hello";
    char *s2 = "World";
    char *s3 = "!";

    libcoll_vector_append(vector, s1);
    ck_assert_uint_eq(vector->length, 1);
    ck_assert_uint_eq(vector->capacity, init_capacity);

    libcoll_vector_append(vector, s2);
    ck_assert_uint_eq(vector->length, 2);
    ck_assert_uint_eq(vector->capacity, init_capacity);

    libcoll_vector_append(vector, s3);
    ck_assert_uint_eq(vector->length, 3);
    ck_assert_uint_gt(vector->capacity, init_capacity);

    ck_assert(libcoll_strcmp_wrapper(s3, (char*) vector->contents[2]) == 0);
    libcoll_vector_deinit(vector);
}
END_TEST

/*
 * Tests the vector pop (return and remove last item) operation.
 */
START_TEST(vector_pop)
{
    DEBUG("\n*** Starting vector_pop\n");

    libcoll_vector_t *vector = libcoll_vector_init();

    int a = 13;
    int b;
    libcoll_vector_append(vector, &a);
    b = *(int*) libcoll_vector_pop(vector);
    ck_assert_int_eq(a, b);
    ck_assert_uint_eq(libcoll_vector_length(vector), 0);
    libcoll_vector_deinit(vector);
}
END_TEST

/*
 * Tests that inserting into a vector works properly, including vector
 * resizing.
 */
START_TEST(vector_insert)
{
    DEBUG("\n*** Starting vector_insert\n");

    size_t init_capacity = 2LU;
    libcoll_vector_t *vector = libcoll_vector_init_with_params(
        init_capacity,
        libcoll_strcmp_wrapper
    );

    char *s1 = "Hello";
    char *s2 = "World";
    char *s3 = "!";

    libcoll_vector_insert(vector, 0, s3);
    libcoll_vector_insert(vector, 0, s1);
    libcoll_vector_insert(vector, 1, s2);

    ck_assert_uint_eq(libcoll_vector_length(vector), 3);

    ck_assert_str_eq((char*) libcoll_vector_get(vector, 0), s1);
    ck_assert_str_eq((char*) libcoll_vector_get(vector, 1), s2);
    ck_assert_str_eq((char*) libcoll_vector_get(vector, 2), s3);

    libcoll_vector_deinit(vector);
}
END_TEST

START_TEST(vector_iterator)
{
    DEBUG("\n*** Starting vector_iterator\n");

    libcoll_vector_t *vector = libcoll_vector_init();

    char *s1 = "Hello";
    char *s2 = "World";
    char *s3 = "!";

    libcoll_vector_append(vector, s1);
    libcoll_vector_append(vector, s2);
    libcoll_vector_append(vector, s3);

    libcoll_vector_iter_t *iter = libcoll_vector_get_iter(vector);

    ck_assert(libcoll_vector_iter_has_next(iter));
    ck_assert(!libcoll_vector_iter_has_previous(iter));

    ck_assert_str_eq(s1, (char*) libcoll_vector_iter_next(iter));
    ck_assert(libcoll_vector_iter_has_next(iter));
    ck_assert(libcoll_vector_iter_has_previous(iter));

    ck_assert_str_eq(s2, (char*) libcoll_vector_iter_next(iter));
    ck_assert(libcoll_vector_iter_has_next(iter));
    ck_assert(libcoll_vector_iter_has_previous(iter));

    ck_assert_str_eq(s3, (char*) libcoll_vector_iter_next(iter));
    ck_assert(!libcoll_vector_iter_has_next(iter));
    ck_assert(libcoll_vector_iter_has_previous(iter));

    /* test traversing back and forth and removing with the iterator from different positions */
    ck_assert_str_eq(s3, (char*) libcoll_vector_iter_previous(iter));
    ck_assert(libcoll_vector_iter_has_next(iter));
    ck_assert(libcoll_vector_iter_has_previous(iter));

    /* traverse back over another element (s2) */
    ck_assert_str_eq(s2, (char*) libcoll_vector_iter_previous(iter));
    ck_assert(libcoll_vector_iter_has_next(iter));
    ck_assert(libcoll_vector_iter_has_previous(iter));

    /* this should remove s2, i.e. element at index 1 */
    libcoll_vector_iter_remove(iter);

    ck_assert_uint_eq(vector->length, 2);
    ck_assert(!libcoll_vector_contains(vector, s2));
    ck_assert(libcoll_vector_iter_has_next(iter));
    ck_assert(libcoll_vector_iter_has_previous(iter));

    ck_assert_str_eq(s3, (char*) libcoll_vector_iter_next(iter));
    ck_assert(!libcoll_vector_iter_has_next(iter));
    ck_assert(libcoll_vector_iter_has_previous(iter));

    /* this should remove s3, i.e. element at index 1 (by now the end of vector) */
    libcoll_vector_iter_remove(iter);

    ck_assert_uint_eq(vector->length, 1);
    ck_assert(!libcoll_vector_contains(vector, s3));
    ck_assert(!libcoll_vector_iter_has_next(iter));
    ck_assert(libcoll_vector_iter_has_previous(iter));

    /* traverse back over remaining element (s1) */
    ck_assert_str_eq(s1, (char*) libcoll_vector_iter_previous(iter));
    ck_assert(libcoll_vector_iter_has_next(iter));
    ck_assert(!libcoll_vector_iter_has_previous(iter));

    /* remove remaining element */
    libcoll_vector_iter_remove(iter);

    ck_assert_uint_eq(vector->length, 0);
    ck_assert(!libcoll_vector_contains(vector, s1));
    ck_assert(!libcoll_vector_iter_has_next(iter));
    ck_assert(!libcoll_vector_iter_has_previous(iter));


    libcoll_vector_drop_iter(iter);
    libcoll_vector_deinit(vector);
}

TCase* create_vector_tests(void)
{
    TCase *tc_core;
    tc_core = tcase_create("vector_core");

    tcase_add_test(tc_core, vector_create);
    tcase_add_test(tc_core, vector_populate_and_retrieve);
    tcase_add_test(tc_core, vector_resize);
    tcase_add_test(tc_core, vector_insert);
    tcase_add_test(tc_core, vector_pop);
    tcase_add_test(tc_core, vector_iterator);

    return tc_core;
}
