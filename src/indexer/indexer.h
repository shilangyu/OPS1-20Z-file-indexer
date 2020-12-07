#ifndef _INDEXER_H
#define _INDEXER_H

#include "../cmd/cmd.h"
#include <pthread.h>
#include <stdint.h>

/// returns TID of the indexer thread
pthread_t start_indexer(args_t args);

/// supported file types
typedef enum {
    INDEX_FILE_TYPE_DIR,
    INDEX_FILE_TYPE_JPEG,
    INDEX_FILE_TYPE_PNG,
    INDEX_FILE_TYPE_GZIP,
    INDEX_FILE_TYPE_ZIP
} index_file_type;

/// type describing an indexed file
typedef struct {
    index_file_type type;
    char *filename;
    char *path;
    size_t size;
    size_t owner_uid;
} indexed_file_t;

#endif
