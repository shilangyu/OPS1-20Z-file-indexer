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
    sigset_t *mask;
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

    char *absolute = realpath(name, NULL);
    if (strlen(name) >= MAX_PATH_BUFFER || strlen(absolute) >= MAX_PATH_BUFFER) {
        printf("Path too long, will be omitted: %s\n", name);
        return 0;
    }

    index_entry_t in = {
        .type      = t,
        .owner_uid = s->st_uid,
        .size      = s->st_size};
    strcpy(in.filename, name);
    strcpy(in.path, absolute);
    free(absolute);

    if (_index_length == _index_capacity) {
        _index_capacity *= 2;
        _index = realloc(_index, sizeof(index_entry_t) * _index_capacity);
    }

    _index[_index_length++] = in;

    return 0;
}

void *indexer(void *arg) {
    struct thread_data *data = (struct thread_data *)arg;

    size_t loaded_length;
    index_entry_t *loaded = load_index(data->args.index_file, &loaded_length);
    if (loaded != NULL) {
        pthread_mutex_lock(&data->state->index_mtx);
        free(data->state->index);
        data->state->index_capacity = loaded_length;
        data->state->index_length   = loaded_length;
        data->state->index          = loaded;
        pthread_mutex_unlock(&data->state->index_mtx);

        printf("Loaded index file with %ld entries.\n", loaded_length);
    }

    bool first_run = true;
    int sig        = SIGREINDEX;
    while (true) {
        if (!first_run && sigwait(data->mask, &sig))
            ERR("sigwait");

        first_run = false;

        switch (sig) {
        case SIGREINDEX:
        case SIGALRM:
            alarm(0);

            _index_length   = 0;
            _index_capacity = data->state->index_capacity;
            _index          = malloc(sizeof(index_entry_t) * _index_capacity);

            pthread_mutex_lock(&data->state->is_building_mtx);
            data->state->is_building = true;
            pthread_mutex_unlock(&data->state->is_building_mtx);

            nftw(data->args.directory, walk, MAX_FD, FTW_PHYS);

            pthread_mutex_lock(&data->state->index_mtx);
            free(data->state->index);
            data->state->index_capacity = _index_capacity;
            data->state->index_length   = _index_length;
            data->state->index          = _index;
            pthread_mutex_unlock(&data->state->index_mtx);

            save_index(data->args.index_file, data->state->index, data->state->index_length);

            pthread_mutex_lock(&data->state->done_saving_mtx);
            pthread_cond_signal(&data->state->done_saving);
            pthread_mutex_unlock(&data->state->done_saving_mtx);

            pthread_mutex_lock(&data->state->is_building_mtx);
            data->state->is_building = false;
            pthread_mutex_unlock(&data->state->is_building_mtx);

            printf("Finished indexing %ld files.\n", data->state->index_length);

            break;

        default:
            fprintf(stderr, "unknown signal\n");
            exit(EXIT_FAILURE);
        }

        if (data->args.rebuild_interval == 0) break;

        alarm(data->args.rebuild_interval);
    }

    return NULL;
}

pthread_t start_indexer(args_t args, mole_state_t *state) {
    pthread_t tid;

    sigset_t *new_mask = malloc(sizeof(sigset_t));
    CHECK(new_mask == NULL);
    sigemptyset(new_mask);
    sigaddset(new_mask, SIGREINDEX);
    sigaddset(new_mask, SIGALRM);
    pthread_sigmask(SIG_BLOCK, new_mask, NULL);

    struct thread_data *data = malloc(sizeof(struct thread_data));
    CHECK(data == NULL);
    data->args  = args;
    data->state = state;
    data->mask  = new_mask;

    CHECK(pthread_create(&tid, NULL, indexer, (void *)data));

    return tid;
}
