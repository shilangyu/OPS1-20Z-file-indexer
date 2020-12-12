#include "repl.h"
#include "../indexer/indexer.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void print_satisfying(index_entry_t *index, size_t index_length, bool (*predicate)(index_entry_t index, void *arg), void *arg) {
    bool has_over_3 = false;
    size_t found    = 0;
    for (size_t i = 0; i < index_length; i++) {
        if (predicate(index[i], arg)) {
            found++;
            if (found == 3) {
                has_over_3 = true;
                break;
            }
        }
    }

    FILE *out = stdout;

    char *pager;
    if (has_over_3 && (pager = getenv("PAGER")) != NULL) {
        out = popen(pager, "w");
        if (out == NULL) {
            ERR("popen");
        }
    }

    for (size_t i = 0; i < index_length; i++) {
        if (predicate(index[i], arg)) {
            fprintf(out, "%s:\n"
                         "\tSize: %ld\n"
                         "\tType: %s\n",
                    index[i].path,
                    index[i].size,
                    index_file_type_repr(index[i].type));
        }
    }

    if (out != stdout) {
        CHECK(pclose(out));
    }
}

static inline bool starts_with_and_more(const char *string, const char *prefix) {
    return strlen(string) > strlen(prefix) &&
           !strncmp(string, prefix, strlen(prefix));
}

command_t read_next() {
    command_t res;
    printf("> ");
// surely that should be enough, right? :^)
#define BUFFER_SIZE 100
    static char buffer[BUFFER_SIZE];
    fgets(buffer, BUFFER_SIZE, stdin);
    strtok(buffer, "\n");

    if (!strcmp(buffer, "exit")) {
        res.type = COMMAND_TYPE_EXIT;
    } else if (!strcmp(buffer, "exit!")) {
        res.type = COMMAND_TYPE_EXIT_FORCE;
    } else if (!strcmp(buffer, "index")) {
        res.type = COMMAND_TYPE_INDEX;
    } else if (!strcmp(buffer, "count")) {
        res.type = COMMAND_TYPE_COUNT;
    } else if (starts_with_and_more(buffer, "largerthan ")) {
        res.param.num = atoi(buffer + strlen("largerthan "));
        res.type      = COMMAND_TYPE_LARGER_THAN;
    } else if (starts_with_and_more(buffer, "namepart ")) {
        res.param.str = strdup(buffer + strlen("namepart "));
        res.type      = COMMAND_TYPE_NAME_PART;
    } else if (starts_with_and_more(buffer, "owner ")) {
        res.param.num = atoi(buffer + strlen("owner "));
        res.type      = COMMAND_TYPE_OWNER;
    } else {
        res.type = COMMAND_TYPE_UNKNOWN;
    }

    return res;
#undef BUFFER_SIZE
}
