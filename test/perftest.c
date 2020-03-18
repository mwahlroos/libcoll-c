#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "helpers.h"
#include "../src/hash.h"
#include "../src/hashmap.h"
#include "../src/treemap.h"
#include "../src/types.h"

#define BENCHMARK_SEED  1U
#define BENCHMARK_SIZE  10000000U
#define KEY_STR_LEN     5

typedef enum {
    NONE,
    HASHMAP,
    TREEMAP
} BenchmarkTarget;

static void randstr(char *buf, size_t len)
{
    char range_start = 'a';
    char range_end = 'z';

    for (size_t i=0; i<len; i++) {
        int r = rand() % (range_end - range_start) + range_start;
        buf[i] = (char) r;
    }
}

static void generate_key_value_data(libcoll_pair_voidptr_t *buf, unsigned long items)
{
    srand(BENCHMARK_SEED);
    size_t strlen = 5;

    for (unsigned long i=0; i<items; i++) {
        char *key = malloc(strlen * sizeof(char));
        int *value = malloc(sizeof(int));

        randstr(key, strlen);
        *value = rand();

        libcoll_pair_voidptr_t kvpair = {key, value};
        buf[i] = kvpair;
    }
}

static void populate_hashmap(libcoll_hashmap_t *hm, unsigned long items)
{
    libcoll_pair_voidptr_t *tmp = malloc(items * sizeof(libcoll_pair_voidptr_t));
    generate_key_value_data(tmp, items);

    for (unsigned long i=0; i<items; i++) {
        libcoll_pair_voidptr_t kvpair = tmp[i];
        libcoll_hashmap_put(hm, kvpair.a, kvpair.b);
    }

    free(tmp);
}

static void populate_treemap(libcoll_treemap_t *tm, unsigned long items)
{
    srand(BENCHMARK_SEED);
    size_t strlen = 5;

    for (unsigned long i=0; i<items; i++) {
        char *key = malloc(strlen * sizeof(char));
        randstr(key, strlen);

        int *value = malloc(sizeof(int));
        *value = rand();

        libcoll_treemap_add(tm, key, value);
    }
}

static void benchmark_hashmap(unsigned long testsize)
{
    libcoll_hashmap_t *hm =
    libcoll_hashmap_init_with_params(
        LIBCOLL_HASHMAP_DEFAULT_INIT_SIZE,
        LIBCOLL_HASHMAP_DEFAULT_MAX_LOAD_FACTOR,
        hashcode_str,
        strcmp_wrapper,
        intptrcmp
    );

    populate_hashmap(hm, testsize);
    // print_hashmap(hm, stdout);
    libcoll_hashmap_deinit(hm);
}

static void benchmark_treemap(unsigned long testsize)
{
    libcoll_treemap_t *tm =
        libcoll_treemap_init_with_comparator(strcmp_wrapper);

    populate_treemap(tm, testsize);
    libcoll_treemap_deinit(tm);
}

int main(int argc, char *argv[])
{
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
