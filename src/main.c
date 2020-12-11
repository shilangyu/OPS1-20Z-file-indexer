#include "main.h"
#include "cmd/cmd.h"
#include "indexer/indexer.h"
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
        .index_mtx       = PTHREAD_MUTEX_INITIALIZER,
        .is_building     = false,
        .is_building_mtx = PTHREAD_MUTEX_INITIALIZER};

    pthread_t indexer_tid = start_indexer(args, &state);

    pthread_join(indexer_tid, NULL);
    return 0;
}

#endif
