#ifndef _MAIN_H
#define _MAIN_H

/// buffer size for all strings that hold filenames/paths
#define PATH_LENGTH_LIMIT 50

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
