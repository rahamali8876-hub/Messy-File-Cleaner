// src/core/fs_interface_validate.c

// #include "cleaner/system/executor.h"
#include <stdio.h>

#define EXEC_REQUIRE(fn)                                                       \
  if (!(exec->fn)) {                                                           \
    fprintf(stderr, "Executor missing %s\n", #fn);                             \
    return -1;                                                                 \
  }
