#if TEST

#include "../cmd/cmd.h"
#include "../indexer/indexer.h"
#include <assert.h>
#include <stdbool.h>
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
    setenv("MOLE_DIR", "placeholder", true);
    {
        char *argv[] = {"molea", "-t", "420"};
        args_t args  = parse_arguments(sizeof(argv) / sizeof(argv[0]), argv);
        assert(args.rebuild_interval == 420);
        assert(strcmp(args.directory, "placeholer"));
    }
    {
        char *argv[] = {"mole", "-d", "somewhere", "-f", "mole/file"};
        args_t args  = parse_arguments(sizeof(argv) / sizeof(argv[0]), argv);
        assert(args.rebuild_interval == 0);
        assert(strcmp(args.directory, "somewhere"));
        assert(strcmp(args.index_file, "mole/file"));
    }
    {
        setenv("MOLE_DIR", "hello", true);
        setenv("MOLE_INDEX_PATH", "world", true);
        char *argv[] = {"mole"};
        args_t args  = parse_arguments(sizeof(argv) / sizeof(argv[0]), argv);
        assert(args.rebuild_interval == 0);
        assert(strcmp(args.directory, "hello"));
        assert(strcmp(args.index_file, "world"));
    }
}

int main() {
    get_file_type_test();
    parse_arguments_test();
}
#endif
