/*
 * comparators.h
 *
 * Functions that can be used as comparators in various collections.
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

/*
 * Comparator for integer keys/values.
 *
 * The pointers are treated as pointers to integers, and the comparison
 * is done based on the integer values they point to.
 */
int libcoll_intptrcmp(const void *value1, const void *value2);

/*
 * Comparator for string keys/values.
 *
 * Wrapper around strcmp that accepts void pointers, for comparing map keys
 * and/or values.
 */
int libcoll_strcmp_wrapper(const void *value1, const void *value2);

/*
 * A comparator function for keys using the memory address of the data as a
 * basis of (in)equality.
 *
 * Used as the default comparator in all collections.
 *
 * Note: The result of comparing pointers is undefined if the pointers do not
 * point to members of the same array, so this is technically not a very good
 * comparator for generic pointers. However, there's no obvious better
 * alternative for a default comparator in C, so considering that this probably
 * practically works at least for (in)equality, which most collections are
 * interested in, it can stay for now.
 *
 * If you're building for a platform where comparing two arbitrary pointers for
 * equality actually causes undefined behaviour, you should use a custom
 * comparator instead.
 */
int libcoll_memaddrcmp(const void *value1, const void *value2);
