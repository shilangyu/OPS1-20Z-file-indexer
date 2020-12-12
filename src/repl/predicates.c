#include "repl.h"
#include <string.h>

bool largerthan_predicate(index_entry_t index, void *arg) {
    return index.size > *(int *)arg;
}

bool namepart_predicate(index_entry_t index, void *arg) {
    return strstr(index.filename, (char *)arg) != NULL;
}

bool owner_predicate(index_entry_t index, void *arg) {
    return index.owner_uid == *(int *)arg;
}
