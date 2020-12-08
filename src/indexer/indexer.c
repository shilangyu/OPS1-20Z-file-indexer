#define _XOPEN_SOURCE 500
#include "./indexer.h"
#include "../cmd/cmd.h"
#include "../main.h"
#include <ftw.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// boo, goddamn C
indexed_file_t _index[INDEX_SIZE];
size_t _curr = 0;

int walk(const char *name, const struct stat *s, int type, struct FTW *f) {
    index_file_type t = get_file_type(name, type);
    if (t == INDEX_FILE_TYPE_UNKNOWN) {
        return 0;
    }

    // TODO: check if paths are too long
    indexed_file_t in = {
        .filename = strcpy(malloc(sizeof(char) * PATH_LENGTH_LIMIT), name),
        .path     = realpath(name, NULL),
        .type     = t,
        // TODO: get this
        .owner_uid = 0,
        // TODO: get this
        .size = 0};

    _index[_curr++] = in;

    return 0;
}

void *indexer(void *arg) {
    nftw(".", walk, MAX_FD, FTW_PHYS);

    for (size_t i = 0; i < _curr; i++) {
        puts(_index[i].filename);
    }

    return NULL;
}

pthread_t start_indexer(args_t args) {
    pthread_t tid;

    CHECK(pthread_create(&tid, NULL, indexer, NULL));

    return tid;
}
