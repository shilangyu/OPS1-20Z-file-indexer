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
#include <sys/mman.h>
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
    CHECK(close(fd));

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

    return INDEX_FILE_TYPE_UNKNOWN;
}

const char *index_file_type_repr(index_file_type type) {
    switch (type) {
    case INDEX_FILE_TYPE_PNG:
        return "PNG image";
        break;
    case INDEX_FILE_TYPE_GZIP:
        return "GZIP archive";
        break;
    case INDEX_FILE_TYPE_ZIP:
        return "ZIP archive";
        break;
    case INDEX_FILE_TYPE_JPEG:
        return "JPEG image";
        break;
    case INDEX_FILE_TYPE_DIR:
        return "directory";
        break;
    case INDEX_FILE_TYPE_UNKNOWN:
        return "unknown";
        break;
    default:
        fprintf(stderr, "unsupported file type\n");
        exit(EXIT_FAILURE);
    }
}

index_entry_t *load_index(const char *path, size_t *index_length) {
    int fd = open(path, O_RDONLY);
    struct stat stats;
    if (fd == -1 || fstat(fd, &stats) == -1) {
        return NULL;
    }

    index_entry_t *index = mmap(NULL, stats.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (index == MAP_FAILED) {
        ERR("mmap");
    }
    *index_length = stats.st_size / sizeof(index_entry_t);

    CHECK(close(fd));

    return index;
}

void save_index(const char *path, index_entry_t *index, size_t index_length) {
    int fd = open(path, O_RDWR | O_CREAT | O_TRUNC, 0666);
    if (fd == -1) {
        ERR("open");
    }

    size_t size = sizeof(index_entry_t) * index_length;
    ftruncate(fd, size);

    index_entry_t *res = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (res == MAP_FAILED) {
        ERR("mmap");
    }

    for (size_t i = 0; i < index_length; i++) {
        res[i] = index[i];
    }

    CHECK(close(fd));
}
