
// include/cleaner/core/config.h
#ifndef CLEANER_CORE_CONFIG_H
#define CLEANER_CORE_CONFIG_H

typedef struct
{
  const char *source;
  const char *target;
  int dry_run;
} cleaner_config_t;

#endif