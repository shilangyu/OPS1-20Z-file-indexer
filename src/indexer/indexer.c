#define _XOPEN_SOURCE 500
#include "./indexer.h"
#include "../cmd/cmd.h"
#include "../main.h"
#include <ftw.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct thread_data {
    args_t args;
    mole_state_t *state;
};

// boo, goddamn C
index_entry_t _index[INDEX_SIZE];
size_t _curr = 0;

int walk(const char *name, const struct stat *s, int type, struct FTW *f) {
    index_file_type t = get_file_type(name, type);
    if (t == INDEX_FILE_TYPE_UNKNOWN) {
        return 0;
    }
    char *filename = malloc(sizeof(char) * PATH_LENGTH_LIMIT);
    if (filename == NULL) ERR("malloc");

    // TODO: check if paths are too long
    index_entry_t in = {
        .filename  = strcpy(filename, name),
        .path      = realpath(name, NULL),
        .type      = t,
        .owner_uid = s->st_uid,
        .size      = s->st_size};

    _index[_curr++] = in;

    return 0;
}

void *indexer(void *arg) {
    struct thread_data *data = (struct thread_data *)arg;

    while (true) {
        pthread_mutex_lock(&data->state->is_building_mtx);
        data->state->is_building = true;
        pthread_mutex_unlock(&data->state->is_building_mtx);

        nftw(data->args.directory, walk, MAX_FD, FTW_PHYS);

        pthread_mutex_lock(&data->state->is_building_mtx);
        data->state->is_building = false;
        pthread_mutex_unlock(&data->state->is_building_mtx);

        printf("Finished indexing %ld files.\n", _curr);

        if (data->args.rebuild_interval == 0) break;

        sleep(data->args.rebuild_interval);
    }

    return NULL;
}

pthread_t start_indexer(args_t args, mole_state_t *state) {
    pthread_t tid;

    struct thread_data *data = malloc(sizeof(struct thread_data));
    CHECK(data == NULL);
    data->args  = args;
    data->state = state;

    CHECK(pthread_create(&tid, NULL, indexer, (void *)data));

    return tid;
}
