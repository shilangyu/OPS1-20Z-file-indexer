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
    pthread_join(start_indexer(args), NULL);

    return 0;
}

#endif
