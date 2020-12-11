#if TEST

#include "../cmd/cmd.h"
#include "../indexer/indexer.h"
#include "../repl/repl.h"
#include <assert.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void get_file_type_test() {
    assert(get_file_type("./assets/adobe_crobat.jpg", -1) == INDEX_FILE_TYPE_JPEG);
    assert(get_file_type("./assets/quieres.png", -1) == INDEX_FILE_TYPE_PNG);
    assert(get_file_type("./assets/wojnarowskim.etap1.tar.gz", -1) == INDEX_FILE_TYPE_GZIP);
    assert(get_file_type("./assets/data.zip", -1) == INDEX_FILE_TYPE_ZIP);
    assert(get_file_type("./assets/wahadlo.docx", -1) == INDEX_FILE_TYPE_ZIP);
    assert(get_file_type("./src/main.c", -1) == INDEX_FILE_TYPE_UNKNOWN);
}

void parse_arguments_test() {
    setenv("MOLE_DIR", "placeholder1", true);
    setenv("MOLE_INDEX_PATH", "placeholder2", true);

    {
        char *argv[] = {"mole"};
        args_t args  = parse_arguments(sizeof(argv) / sizeof(argv[0]), argv);
        assert(args.rebuild_interval == 0);
        assert(!strcmp(args.directory, "placeholder1"));
        assert(!strcmp(args.index_file, "placeholder2"));
        optind = 1;
    }

    {
        char *argv[] = {"mole", "-d", "somewhere", "-f", "mole/file", "-t", "420"};
        args_t args  = parse_arguments(sizeof(argv) / sizeof(argv[0]), argv);
        assert(args.rebuild_interval == 420);
        assert(!strcmp(args.directory, "somewhere"));
        assert(!strcmp(args.index_file, "mole/file"));
        optind = 1;
    }

    {
        unsetenv("MOLE_INDEX_PATH");
        char *home = getenv("HOME");
        char *path = "/.mole-index";
        char *res  = calloc((strlen(home) + strlen(path) + 1), sizeof(char));
        strcat(res, home);
        strcat(res, path);

        char *argv[] = {"mole"};
        args_t args  = parse_arguments(sizeof(argv) / sizeof(argv[0]), argv);
        assert(!strcmp(args.index_file, res));
        optind = 1;
    }
}

void index_file_type_repr_test() {
    assert(!strcmp(index_file_type_repr(INDEX_FILE_TYPE_DIR), "directory"));
    assert(!strcmp(index_file_type_repr(INDEX_FILE_TYPE_GZIP), "GZIP archive"));
    assert(!strcmp(index_file_type_repr(INDEX_FILE_TYPE_JPEG), "JPEG image"));
    assert(!strcmp(index_file_type_repr(INDEX_FILE_TYPE_PNG), "PNG image"));
    assert(!strcmp(index_file_type_repr(INDEX_FILE_TYPE_UNKNOWN), "unknown"));
    assert(!strcmp(index_file_type_repr(INDEX_FILE_TYPE_ZIP), "ZIP archive"));
}

void read_next_test() {
    // feed in inputs from assets/test-inputs.txt

    {
        command_t cmd = read_next();
        assert(cmd.type == COMMAND_TYPE_UNKNOWN);
    }

    {
        command_t cmd = read_next();
        assert(cmd.type == COMMAND_TYPE_COUNT);
    }

    {
        command_t cmd = read_next();
        assert(cmd.type == COMMAND_TYPE_EXIT);
    }

    {
        command_t cmd = read_next();
        assert(cmd.type == COMMAND_TYPE_EXIT_FORCE);
    }

    {
        command_t cmd = read_next();
        assert(cmd.type == COMMAND_TYPE_INDEX);
    }

    {
        command_t cmd = read_next();
        assert(cmd.type == COMMAND_TYPE_LARGER_THAN);
        assert(cmd.param.num == 123);
    }

    {
        command_t cmd = read_next();
        assert(cmd.type == COMMAND_TYPE_NAME_PART);
        assert(!strcmp(cmd.param.str, "hello"));
    }

    {
        command_t cmd = read_next();
        assert(cmd.type == COMMAND_TYPE_OWNER);
        assert(cmd.param.num == 321);
    }
}

int main() {
    get_file_type_test();
    parse_arguments_test();
    index_file_type_repr_test();
    read_next_test();
}
#endif
