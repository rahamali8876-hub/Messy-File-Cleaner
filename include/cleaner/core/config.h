
// include/cleaner/core/config.h

#ifndef CLEANER_CORE_CONFIG_H
#define CLEANER_CORE_CONFIG_H

#include "cleaner/platform/filesystem/filesystem_win.h"
#include "cleaner/system/executor.h"

typedef struct {
  const char *source;
  const char *target;
  int dry_run;
} cleaner_config_t;

#endif