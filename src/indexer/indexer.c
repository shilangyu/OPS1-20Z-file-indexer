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
index_entry_t *_index;
size_t _index_length;
size_t _index_capacity;

int walk(const char *name, const struct stat *s, int type, struct FTW *f) {
    index_file_type t = get_file_type(name, type);
    if (t == INDEX_FILE_TYPE_UNKNOWN) {
        return 0;
    }

    index_entry_t in = {
        .filename  = strdup(name),
        .path      = realpath(name, NULL),
        .type      = t,
        .owner_uid = s->st_uid,
        .size      = s->st_size};

    if (_index_length == _index_capacity) {
        _index_capacity *= 2;
        _index = realloc(_index, sizeof(index_entry_t) * _index_capacity);
    }

    _index[_index_length++] = in;

    return 0;
}

void *indexer(void *arg) {
    struct thread_data *data = (struct thread_data *)arg;

    while (true) {
        _index_length   = 0;
        _index_capacity = data->state->index_capacity;
        _index          = malloc(sizeof(index_entry_t) * _index_capacity);

        pthread_mutex_lock(&data->state->is_building_mtx);
        data->state->is_building = true;
        pthread_mutex_unlock(&data->state->is_building_mtx);

        nftw(data->args.directory, walk, MAX_FD, FTW_PHYS);

        pthread_mutex_lock(&data->state->index_mtx);
        data->state->index_capacity = _index_capacity;
        data->state->index_length   = _index_length;
        // TODO: memory leak, should cleanup data->state->index first
        data->state->index = _index;
        pthread_mutex_unlock(&data->state->index_mtx);

        pthread_mutex_lock(&data->state->is_building_mtx);
        data->state->is_building = false;
        pthread_mutex_unlock(&data->state->is_building_mtx);

        printf("Finished indexing %ld files.\n", data->state->index_length);

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
