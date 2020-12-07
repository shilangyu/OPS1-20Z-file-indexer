#ifndef _CMD_H
#define _CMD_H

#include <stdint.h>

/// parsed CLI arguments
typedef struct {
  char* directory;
  char* index_file;
  /// in seconds
  uint16_t rebuild_interval;
} args_t;

/// if parameters are incorrect prints help message and exits
/// otherwise returns the parsed arguments
args_t parse_arguments(int argc, char** argv);

#endif
