#define _XOPEN_SOURCE 500
#include "../cmd/cmd.h"
#include "../main.h"
#include "./indexer.h"
#include <ftw.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

index_file_type get_file_type(const char *filename, int type) {
    if (type == FTW_DNR || type == FTW_D) {
        return INDEX_FILE_TYPE_DIR;
    }

    return INDEX_FILE_TYPE_JPEG;
}
