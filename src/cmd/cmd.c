// #define _GNU_SOURCE
#include "cmd.h"
#include "../main.h"
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void help_exit(char *prog_name) {
    fprintf(stderr,
            "File indexing program\n"
            "Marcin Wojnarowski <303880>\n"
            "\n"
            "USAGE:\n"
            "\t%s [OPTIONS]\n"
            "\n"
            "OPTIONS:\n"
            "\t-d <PATH>   A path to a directory that will be traversed [default: $MOLE_DIR]\n"
            "\t-f <PATH>   A path to a file where index is stored [default: $MOLE_INDEX_PATH]\n"
            "\t-t <n>      Seconds between subsequent rebuilds of index in the [30, 7200] range [default: 0 (never)]\n",
            prog_name);
    exit(EXIT_FAILURE);
}

args_t parse_arguments(int argc, char **argv) {
    args_t args = {
        .directory        = NULL,
        .index_file       = NULL,
        .rebuild_interval = 0};

    int opt;
    while ((opt = getopt(argc, argv, "d:f:t:")) != -1) {
        switch (opt) {
        case 'd':
            args.directory = optarg;
            break;
        case 'f':
            args.index_file = optarg;
            break;
        case 't':
            args.rebuild_interval = atoi(optarg);
            break;
        case '?':
        default:
            help_exit(argv[0]);
        }
    }

    // set fallbacks/error check
    if (args.directory == NULL) {
        args.directory = getenv("MOLE_DIR");

        if (args.directory == NULL) {
            fprintf(stderr, "$MOLE_DIR does not exists nor was the -d option passed\n");
            exit(EXIT_FAILURE);
        }
    }
    if (args.index_file == NULL) {
        args.index_file = getenv("MOLE_INDEX_PATH");

        if (args.index_file == NULL) {
            char *home = getenv("HOME");
            char *path = "/.mole-index";

            args.index_file = calloc((strlen(home) + strlen(path) + 1), sizeof(char));
            strcat(args.index_file, home);
            strcat(args.index_file, path);
        }
    }
    if (args.rebuild_interval != 0 && (args.rebuild_interval < 30 || args.rebuild_interval > 7200)) {
        fprintf(stderr, "-t is not in range\n");
        exit(EXIT_FAILURE);
    }

    return args;
}
