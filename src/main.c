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
        case COMMAND_TYPE_COUNT:

            break;
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
