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
    INDEX_FILE_TYPE_ZIP,
    INDEX_FILE_TYPE_UNKNOWN
} index_file_type;

/// string representation of index_file_type
char *index_file_type_repr(index_file_type type);

/// reads the file signature to determine file type
index_file_type get_file_type(const char *filename, int type);

/// type describing an indexed file
typedef struct {
    index_file_type type;
    char *filename;
    char *path;
    size_t size;
    size_t owner_uid;
} index_entry_t;

#endif
