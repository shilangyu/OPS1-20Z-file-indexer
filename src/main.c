#include "cmd/cmd.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    args_t args = parse_arguments(argc, argv);
    printf("d = %s, f = %s, t = %d\n", args.directory, args.index_file, args.rebuild_interval);
    return 0;
}
