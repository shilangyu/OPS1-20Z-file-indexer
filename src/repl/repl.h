#ifndef _REPL_H
#define _REPL_H

#include "../indexer/indexer.h"
#include <stdbool.h>

/// supported commands
typedef enum {
    COMMAND_TYPE_EXIT,
    COMMAND_TYPE_EXIT_FORCE,
    COMMAND_TYPE_INDEX,
    COMMAND_TYPE_COUNT,
    COMMAND_TYPE_LARGER_THAN,
    COMMAND_TYPE_NAME_PART,
    COMMAND_TYPE_OWNER,
    COMMAND_TYPE_UNKNOWN
} command_type;

/// a command with its param
typedef struct {
    command_type type;
    union {
        int num;
        char *str;
    } param;
} command_t;

/// reads input from stdin and returns the parsed command.
command_t read_next();

/// given the index and a filtering predicate, will print all matching index entries
void print_satisfying(index_entry_t *index, size_t index_length, bool (*predicate)(index_entry_t index, void *arg), void *arg);

bool largerthan_predicate(index_entry_t index, void *arg);
bool namepart_predicate(index_entry_t index, void *arg);
bool owner_predicate(index_entry_t index, void *arg);

#endif
