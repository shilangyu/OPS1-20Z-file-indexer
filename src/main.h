#ifndef _MAIN_H
#define _MAIN_H

#include <signal.h>

/// max amount of file descriptors used during indexing
#define MAX_FD 30

/// max buffer size of string for paths in the index
#define MAX_PATH_BUFFER 300

/// signal for a forced reindex
#define SIGREINDEX SIGUSR1

/// used for unrecoverable errors (panics)
#define ERR(source)                                                  \
    (perror(source), fprintf(stderr, "%s:%d\n", __FILE__, __LINE__), \
     exit(EXIT_FAILURE))

/// panics if a function fails (returns a non-zero integer)
#define CHECK(source)                    \
    do {                                 \
        if ((source) != 0) ERR(#source); \
    } while (0)

#endif
