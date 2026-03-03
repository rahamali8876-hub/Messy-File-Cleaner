
// cleaner/include\processor.h

#ifndef CLEANER_CORE_PROCESSOR_H
#define CLEANER_CORE_PROCESSOR_H

#include "fs_interface.h"
#include "renamer.h"
#include <stdint.h>

typedef struct {
  fs_interface_t fs;
  int (*get_time)(renamer_timestamp_t *out);
  int dry_run;
} processor_t;

/* Initialize */
int processor_init(processor_t *p, const fs_interface_t *fs,
                   int (*get_time_fn)(renamer_timestamp_t *), int dry_run);

/* Process ONE file */
int processor_process_file(processor_t *p, const fs_entry_t *entry,
                           uint64_t id);

#endif