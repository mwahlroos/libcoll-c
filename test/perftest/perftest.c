#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../helpers.h"
#include "hash.h"
#include "hashmap.h"
#include "treemap.h"
#include "types.h"

#define BENCHMARK_SEED                  1U
#define BENCHMARK_SIZE                  10000000LU
#define KEY_STR_LEN                     5
#define BENCHMARK_RETRIEVE_PROPORTION   1  /* one in how many inserted values to retrieve */

typedef enum {
    NONE,
    HASHMAP,
    TREEMAP
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

static void benchmark_hashmap(size_t testsize)
{
    clock_t start_time;
    size_t retrieve_count = BENCHMARK_SIZE / BENCHMARK_RETRIEVE_PROPORTION;

    /* null output for printing values retrieved during retrieval tests,
     * to prevent the compiler from optimizing the retrievals out
     */
    FILE *null_out = get_null_output();

    libcoll_hashmap_t *map =
        libcoll_hashmap_init_with_params(
            LIBCOLL_HASHMAP_DEFAULT_INIT_SIZE,
            LIBCOLL_HASHMAP_DEFAULT_MAX_LOAD_FACTOR,
            hashcode_str,
            strcmp_wrapper,
            intptrcmp
        );

    libcoll_pair_voidptr_t *data = malloc(testsize * sizeof(libcoll_pair_voidptr_t));
    generate_key_value_data(data, testsize);

    printf("Populating a hashmap with %lu entries... \t", testsize);

    start_time = clock();
    populate_hashmap(map, data, testsize);
    printf("%.2f s\n", (double) ((clock() - start_time) / CLOCKS_PER_SEC));

    start_time = clock();
    printf("Retrieving %lu items... \t", retrieve_count);
    for (size_t i=0; i<retrieve_count; i++) {
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
    printf("%.2f s\n", (double) ((clock() - start_time) / CLOCKS_PER_SEC));

    fclose(null_out);
    free(data);

    libcoll_hashmap_deinit(map);
}

static void benchmark_treemap(size_t testsize)
{
    clock_t start_time;
    size_t retrieve_count = BENCHMARK_SIZE / BENCHMARK_RETRIEVE_PROPORTION;

    /* null output for printing values retrieved during retrieval tests,
     * to prevent the compiler from optimizing the retrievals out
     */
    FILE *null_out = get_null_output();

    libcoll_treemap_t *map =
        libcoll_treemap_init_with_comparator(strcmp_wrapper);

    libcoll_pair_voidptr_t *data = malloc(testsize * sizeof(libcoll_pair_voidptr_t));
    generate_key_value_data(data, testsize);

    printf("Populating a treemap with %lu entries... \t", testsize);

    start_time = clock();
    populate_treemap(map, data, testsize);
    printf("%.2f s\n", (double) ((clock() - start_time) / CLOCKS_PER_SEC));

    start_time = clock();
    printf("Retrieving %lu items... \t", retrieve_count);
    for (size_t i=0; i<retrieve_count; i++) {
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
    printf("%.2f s\n", (double) ((clock() - start_time) / CLOCKS_PER_SEC));

    fclose(null_out);
    free(data);

    libcoll_treemap_deinit(map);
}

int main(int argc, char *argv[])
{
    /* disable buffering for stdout so that partial output lines get printed
     * without needing to wait for a line feed first
     */
    setbuf(stdout, NULL);

    BenchmarkTarget target = NONE;

    if (argc > 1) {
        char *s = argv[1];
        if (strcmp(s, "hashmap") == 0) {
            target = HASHMAP;
        } else if (strcmp(s, "treemap") == 0) {
            target = TREEMAP;
        }
    }

    switch (target) {
        case HASHMAP:
            benchmark_hashmap(BENCHMARK_SIZE);
            break;
        case TREEMAP:
            benchmark_treemap(BENCHMARK_SIZE);
            break;
        case NONE:
            fprintf(stderr, "No benchmark selected\n");
            break;
    }
}
