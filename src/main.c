#include "main.h"
#include "cmd/cmd.h"
#include "indexer/indexer.h"
#include "repl/repl.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef TEST

int main(int argc, char *argv[]) {
    args_t args = parse_arguments(argc, argv);
    printf("d = %s, f = %s, t = %ld\n", args.directory, args.index_file, args.rebuild_interval);

    mole_state_t state = {
        .index           = NULL,
        .index_length    = 0,
        .index_capacity  = 32,
        .index_mtx       = PTHREAD_MUTEX_INITIALIZER,
        .is_building     = false,
        .is_building_mtx = PTHREAD_MUTEX_INITIALIZER,
        .done_saving     = PTHREAD_COND_INITIALIZER,
        .done_saving_mtx = PTHREAD_MUTEX_INITIALIZER};
    state.index = malloc(sizeof(index_entry_t) * state.index_capacity);

    pthread_t indexer_tid = start_indexer(args, &state);

    while (1) {
        command_t cmd = read_next();

        switch (cmd.type) {
        case COMMAND_TYPE_EXIT:
            puts("Waiting for any unfinished work...");
            pthread_mutex_lock(&state.is_building_mtx);
            if (state.is_building) {
                pthread_mutex_unlock(&state.is_building_mtx);

                pthread_mutex_lock(&state.done_saving_mtx);
                pthread_cond_wait(&state.done_saving, &state.done_saving_mtx);
                pthread_mutex_unlock(&state.done_saving_mtx);
            } else {
                pthread_mutex_unlock(&state.is_building_mtx);
            }

            exit(EXIT_SUCCESS);

            break;
        case COMMAND_TYPE_EXIT_FORCE:
            puts("Waiting for any unfinished writes...");

            pthread_mutex_lock(&state.done_saving_mtx);
            exit(EXIT_SUCCESS);

            break;
        case COMMAND_TYPE_INDEX:
            pthread_mutex_lock(&state.is_building_mtx);
            if (state.is_building) {
                puts("Indexer is in the midst of an indexing.");
            } else {
                CHECK(pthread_kill(indexer_tid, SIGREINDEX));
            }
            pthread_mutex_unlock(&state.is_building_mtx);

            break;
        case COMMAND_TYPE_COUNT: {
            int dirs  = 0;
            int jpgs  = 0;
            int pngs  = 0;
            int gzips = 0;
            int zips  = 0;

            pthread_mutex_lock(&state.index_mtx);
            for (size_t i = 0; i < state.index_length; i++) {
                switch (state.index[i].type) {
                case INDEX_FILE_TYPE_DIR:
                    dirs++;
                    break;
                case INDEX_FILE_TYPE_JPEG:
                    jpgs++;
                    break;
                case INDEX_FILE_TYPE_PNG:
                    pngs++;
                    break;
                case INDEX_FILE_TYPE_GZIP:
                    gzips++;
                    break;
                case INDEX_FILE_TYPE_ZIP:
                    zips++;
                    break;
                default:
                    break;
                }
            }
            pthread_mutex_unlock(&state.index_mtx);

            printf("%s: %d\n", index_file_type_repr(INDEX_FILE_TYPE_DIR), dirs);
            printf("%s: %d\n", index_file_type_repr(INDEX_FILE_TYPE_JPEG), jpgs);
            printf("%s: %d\n", index_file_type_repr(INDEX_FILE_TYPE_PNG), pngs);
            printf("%s: %d\n", index_file_type_repr(INDEX_FILE_TYPE_GZIP), gzips);
            printf("%s: %d\n", index_file_type_repr(INDEX_FILE_TYPE_ZIP), zips);

            break;
        }
        case COMMAND_TYPE_LARGER_THAN:
            pthread_mutex_lock(&state.index_mtx);
            print_satisfying(state.index, state.index_length, largerthan_predicate, (void *)&cmd.param.num);
            pthread_mutex_unlock(&state.index_mtx);
            break;
        case COMMAND_TYPE_NAME_PART:
            pthread_mutex_lock(&state.index_mtx);
            print_satisfying(state.index, state.index_length, namepart_predicate, (void *)cmd.param.str);
            pthread_mutex_unlock(&state.index_mtx);
            free(cmd.param.str);
            break;
        case COMMAND_TYPE_OWNER:
            pthread_mutex_lock(&state.index_mtx);
            print_satisfying(state.index, state.index_length, owner_predicate, (void *)&cmd.param.num);
            pthread_mutex_unlock(&state.index_mtx);
            break;
        case COMMAND_TYPE_UNKNOWN:
            puts("Unknown command");
            break;
        }
    }

    pthread_join(indexer_tid, NULL);
    return 0;
}

#endif
