#ifndef _MAIN_H
#define _MAIN_H

/// buffer size for all strings that hold filenames/paths
#define PATH_LENGTH_LIMIT 500

/// max amount of file descriptors used during indexing
#define MAX_FD 30

/// max amount of indexed files
//TODO: remove the limit and implement a vector on the index
#define INDEX_SIZE 3000

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
