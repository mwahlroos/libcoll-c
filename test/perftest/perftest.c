/*
 * Basic performance test code for libcoll.
 *
 * The purpose of these tests is mainly to have some basic facilities for
 * evaluating whether e.g. optimizations or other changes made to the library
 * code have an impact on the performance of basic operations on collections.
 *
 * They are not inteded as a full-fledged benchmark suite, or, in their present
 * state, representative of any particular real-world workload.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>  /* for getopt, POSIX-specific */
#include <getopt.h>  /* for getopt, POSIX-specific */

#include "../helpers.h"

#include "comparators.h"
#include "hash.h"
#include "hashmap.h"
#include "treemap.h"
#include "types.h"
#include "vector.h"

#define BENCHMARK_SEED                  1U
#define BENCHMARK_SIZE_DEFAULT          10000000LU
#define BENCHMARK_RUNS_DEFAULT          1
#define KEY_STR_LEN                     5
#define BENCHMARK_RETRIEVE_PROPORTION   1  /* one in how many inserted values to get in retrieval tests */

typedef enum {
    NONE,
    HASHMAP,
    TREEMAP,
    VECTOR
} BenchmarkTarget;

static FILE* get_null_output()
{
    return fopen("/dev/null", "a");
}

static void randstr(char *buf, size_t len)
{
    char range_start = 'a';
    char range_end = 'z';

    for (size_t i=0; i<len; i++) {
        int r = rand() % (range_end - range_start) + range_start;
        buf[i] = (char) r;
    }
}

static void generate_key_value_data(libcoll_pair_voidptr_t *buf, size_t n)
{
    srand(BENCHMARK_SEED);
    size_t strlen = 5;

    for (unsigned long i=0; i<n; i++) {
        char *key = malloc(strlen * sizeof(char));
        int *value = malloc(sizeof(int));

        randstr(key, strlen);
        *value = rand();

        libcoll_pair_voidptr_t kvpair = {key, value};
        buf[i] = kvpair;
    }
}

static void populate_hashmap(libcoll_hashmap_t *hm, libcoll_pair_voidptr_t *data, size_t n)
{
    for (size_t i=0; i<n; i++) {
        libcoll_pair_voidptr_t kvpair = data[i];
        libcoll_hashmap_put(hm, kvpair.a, kvpair.b);
    }
}

static void populate_treemap(libcoll_treemap_t *tm, libcoll_pair_voidptr_t *data, size_t n)
{
    for (size_t i=0; i<n; i++) {
        libcoll_pair_voidptr_t kvpair = data[i];
        libcoll_treemap_add(tm, kvpair.a, kvpair.b);
    }
}

static void populate_vector(libcoll_vector_t *v, libcoll_pair_voidptr_t *data, size_t n)
{
    for (size_t i=0; i<n; i++) {
        libcoll_vector_append(v, data + i);
    }
}

static void benchmark_hashmap(unsigned long testsize)
{
    clock_t start_time;
    unsigned long retrieve_count = testsize / BENCHMARK_RETRIEVE_PROPORTION;

    /* null output for printing values retrieved during retrieval tests,
     * to prevent the compiler from optimizing the retrievals out
     */
    FILE *null_out = get_null_output();

    libcoll_hashmap_t *map =
        libcoll_hashmap_init_with_params(
            LIBCOLL_HASHMAP_DEFAULT_INIT_SIZE,
            LIBCOLL_HASHMAP_DEFAULT_MAX_LOAD_FACTOR,
            libcoll_hashcode_str,
            libcoll_strcmp_wrapper,
            libcoll_intptrcmp
        );

    libcoll_pair_voidptr_t *data = malloc(testsize * sizeof(libcoll_pair_voidptr_t));
    generate_key_value_data(data, testsize);

    printf("Populating a hashmap with %lu entries... \t", testsize);

    start_time = clock();
    populate_hashmap(map, data, testsize);
    printf("%.3f s\n", ((double) (clock() - start_time) / CLOCKS_PER_SEC));

    start_time = clock();
    printf("Retrieving %lu items... \t", retrieve_count);
    for (unsigned long i=0; i<retrieve_count; i++) {
        size_t key_idx = i * (BENCHMARK_RETRIEVE_PROPORTION);
        int *value = (int*) libcoll_hashmap_get(map, data[key_idx].a);

        /* This can be used to print the retrieved value to null output
         * in case the compiler attempts to optimize out the whole thing
         * due to the retrieved value being unused.
         *
         * It takes time and alters the measured results, though.
         */
        // fprintf(null_out, "%d", *value);
    }
    printf("%.3f s\n", ((double) (clock() - start_time) / CLOCKS_PER_SEC));

    fclose(null_out);
    free(data);

    libcoll_hashmap_deinit(map);
}

static void benchmark_treemap(unsigned long testsize)
{
    clock_t start_time;
    unsigned long retrieve_count = testsize / BENCHMARK_RETRIEVE_PROPORTION;

    /* null output for printing values retrieved during retrieval tests,
     * to prevent the compiler from optimizing the retrievals out
     */
    FILE *null_out = get_null_output();

    libcoll_treemap_t *map =
        libcoll_treemap_init_with_comparator(libcoll_strcmp_wrapper);

    libcoll_pair_voidptr_t *data = malloc(testsize * sizeof(libcoll_pair_voidptr_t));
    generate_key_value_data(data, testsize);

    printf("Populating a treemap with %lu entries... \t", testsize);

    start_time = clock();
    populate_treemap(map, data, testsize);
    printf("%.3f s\n", ((double) (clock() - start_time) / CLOCKS_PER_SEC));

    start_time = clock();
    printf("Retrieving %lu items... \t", retrieve_count);
    for (unsigned long i=0; i<retrieve_count; i++) {
        size_t key_idx = i * (BENCHMARK_RETRIEVE_PROPORTION);
        int *value = (int*) libcoll_treemap_get(map, data[key_idx].a);

        /* This can be used to print the retrieved value to null output
         * in case the compiler attempts to optimize out the whole thing
         * due to the retrieved value being unused.
         *
         * It takes time and alters the measured results, though.
         */
        // fprintf(null_out, "%d", *value);
    }
    printf("%.3f s\n", ((double) (clock() - start_time) / CLOCKS_PER_SEC));

    fclose(null_out);
    free(data);

    libcoll_treemap_deinit(map);
}

static void benchmark_vector(unsigned long testsize)
{
    clock_t start_time;

    libcoll_vector_t *vect = libcoll_vector_init_with_params(1, libcoll_memaddrcmp);

    libcoll_pair_voidptr_t *data = malloc(testsize * sizeof(libcoll_pair_voidptr_t));
    generate_key_value_data(data, testsize);

    printf("Appending %lu values into an empty vector, one by one... \t", testsize);
    start_time = clock();
    populate_vector(vect, data, testsize);
    printf("%.3f s\n", ((double) (clock() - start_time) / CLOCKS_PER_SEC));

    free(data);

    libcoll_vector_deinit(vect);
}

int main(int argc, char *argv[])
{
    /* disable buffering for stdout so that partial output lines get printed
     * without needing to wait for a line feed first
     */
    setbuf(stdout, NULL);

    BenchmarkTarget target = NONE;

    /* parse options given on the command line */
    int option_char;
    long benchmark_size = BENCHMARK_SIZE_DEFAULT;
    int benchmark_runs = BENCHMARK_RUNS_DEFAULT;

    while ((option_char = getopt(argc, argv, "n:t:")) != -1) {
        switch (option_char) {
            case 'n':
                if (sscanf(optarg, "%ld", &benchmark_size) != 1 || benchmark_size <= 0) {
                    fprintf(stderr, "-n requires a positive integer argument\n");
                    return EXIT_FAILURE;
                }
                break;
            case 't':
                if (sscanf(optarg, "%d", &benchmark_runs) != 1 || benchmark_runs <= 0) {
                    fprintf(stderr, "-t requires a positive integer argument\n");
                    return EXIT_FAILURE;
                }
                break;
            case '?':
                /* invalid option, either unknown or lacking a required argument */
                switch (optopt) {
                    case 'n':
                    case 't':
                        fprintf(stderr, "-%c requires a positive integer argument\n", optopt);
                        return EXIT_FAILURE;
                    default:
                        fprintf(stderr, "Unknown option\n");
                        return EXIT_FAILURE;
                }
        }
    }

    if (argc > optind) {
        char *s = argv[optind];
        if (strcmp(s, "hashmap") == 0) {
            target = HASHMAP;
        } else if (strcmp(s, "treemap") == 0) {
            target = TREEMAP;
        } else if (strcmp(s, "vector") == 0) {
            target = VECTOR;
        }
    }

    switch (target) {
        case HASHMAP:
            for (int i=0; i<benchmark_runs; i++) {
                printf("Benchmark run %u\n", i+1);
                benchmark_hashmap(benchmark_size);
            }
            break;
        case TREEMAP:
            for (int i=0; i<benchmark_runs; i++) {
                printf("Benchmark run %u\n", i+1);
                benchmark_treemap(benchmark_size);
            }
            break;
        case VECTOR:
            for (int i=0; i<benchmark_runs; i++) {
                printf("Benchmark run %u\n", i+1);
                benchmark_vector(benchmark_size);
            }
            break;
        case NONE:
        default:
            fprintf(stderr, "No benchmark selected\n");
            break;
    }
}
