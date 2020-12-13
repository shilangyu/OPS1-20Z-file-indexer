#ifndef _INDEXER_H
#define _INDEXER_H

#include "../cmd/cmd.h"
#include "../main.h"
#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>

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
const char *index_file_type_repr(index_file_type type);

/// reads the file signature to determine file type
index_file_type get_file_type(const char *filename, int type);

/// type describing an indexed file
typedef struct {
    index_file_type type;
    char filename[MAX_PATH_BUFFER];
    char path[MAX_PATH_BUFFER];
    size_t size;
    size_t owner_uid;
} index_entry_t;

/// state of the whole app
typedef struct {
    bool is_building;
    pthread_mutex_t is_building_mtx;

    bool is_writing;
    pthread_mutex_t is_writing_mtx;

    index_entry_t *index;
    size_t index_length;
    size_t index_capacity;
    pthread_mutex_t index_mtx;
} mole_state_t;

/// returns TID of the indexer thread
pthread_t start_indexer(args_t args, mole_state_t *state);

#endif
