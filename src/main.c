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
    printf("d = %s, f = %s, t = %d\n", args.directory, args.index_file, args.rebuild_interval);

    mole_state_t state = {
        .index           = NULL,
        .index_length    = 0,
        .index_capacity  = 32,
        .index_mtx       = PTHREAD_MUTEX_INITIALIZER,
        .is_building     = false,
        .is_building_mtx = PTHREAD_MUTEX_INITIALIZER};
    state.index = malloc(sizeof(index_entry_t) * state.index_capacity);

    pthread_t indexer_tid = start_indexer(args, &state);

    while (1) {
        command_t cmd = read_next();

        switch (cmd.type) {
        case COMMAND_TYPE_EXIT:
            puts("Waiting for the current indexing...");
            // TODO: also wait for file write
            while (1) {
                pthread_mutex_lock(&state.is_building_mtx);
                if (!state.is_building) break;
                pthread_mutex_unlock(&state.is_building_mtx);
            }
            exit(EXIT_SUCCESS);
            break;
        case COMMAND_TYPE_EXIT_FORCE:

            break;
        case COMMAND_TYPE_INDEX:

            break;
        case COMMAND_TYPE_COUNT: {
            int dirs  = 0;
            int jpgs  = 0;
            int pngs  = 0;
            int gzips = 0;
            int zips  = 0;
            // TODO: should not block actually, indexing should work on a working copy
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

            break;
        case COMMAND_TYPE_NAME_PART:

            break;
        case COMMAND_TYPE_OWNER:

            break;
        case COMMAND_TYPE_UNKNOWN:

            break;
        }
    }

    pthread_join(indexer_tid, NULL);
    return 0;
}

#endif
