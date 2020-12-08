#define _XOPEN_SOURCE 500
#include "../cmd/cmd.h"
#include "../main.h"
#include "./indexer.h"
#include <fcntl.h>
#include <ftw.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

index_file_type get_file_type(const char *filename, int type) {
    if (type == FTW_DNR || type == FTW_D) {
        return INDEX_FILE_TYPE_DIR;
    }

    int fd;
    CHECK((fd = open(filename, O_RDONLY)) == -1);

    // 12 because that is the longest signature
    static const size_t SIG_SIZE = 12;
    uint8_t buffer[SIG_SIZE];
    CHECK(read(fd, buffer, SIG_SIZE) == -1);

// helper for checking signatures
#define CHECK_SIGNATURE(then_return, ...)                               \
    do {                                                                \
        uint8_t signature[] = {__VA_ARGS__};                            \
        size_t n            = sizeof(signature) / sizeof(signature[0]); \
        bool is_that        = true;                                     \
        for (size_t i = 0; i < n; i++) {                                \
            if (buffer[i] != signature[i]) {                            \
                is_that = false;                                        \
                break;                                                  \
            }                                                           \
        }                                                               \
        if (is_that) return (then_return);                              \
    } while (0)

    // Signatures taken from: https://en.wikipedia.org/wiki/List_of_file_signatures
    CHECK_SIGNATURE(INDEX_FILE_TYPE_PNG, 0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A);
    CHECK_SIGNATURE(INDEX_FILE_TYPE_GZIP, 0x1F, 0x8B);
    CHECK_SIGNATURE(INDEX_FILE_TYPE_ZIP, 0x50, 0x4B, 0x03, 0x04);
    CHECK_SIGNATURE(INDEX_FILE_TYPE_ZIP, 0x50, 0x4B, 0x05, 0x06);
    CHECK_SIGNATURE(INDEX_FILE_TYPE_ZIP, 0x50, 0x4B, 0x07, 0x08);
    CHECK_SIGNATURE(INDEX_FILE_TYPE_JPEG, 0xFF, 0xD8, 0xFF, 0xDB);
    CHECK_SIGNATURE(INDEX_FILE_TYPE_JPEG, 0xFF, 0xD8, 0xFF, 0xE0, 0x00, 0x10, 0x4A, 0x46, 0x49, 0x46, 0x00, 0x01);
    CHECK_SIGNATURE(INDEX_FILE_TYPE_JPEG, 0xFF, 0xD8, 0xFF, 0xEE);
    // TODO: wtf are ?? in file signature?
    // CHECK_SIGNATURE(INDEX_FILE_TYPE_JPEG, {0xFF, 0xD8, 0xFF, 0xE1, 0x??, 0x??, 0x45, 0x78, 0x69, 0x66, 0x00, 0x00});

#undef CHECK_SIGNATURE

    CHECK(close(fd));

    return INDEX_FILE_TYPE_UNKNOWN;
}
