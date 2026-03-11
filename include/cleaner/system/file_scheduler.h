// include/cleaner/system/file_scheduler.h

#ifndef CLEANER_FILE_SCHEDULER_H
#define CLEANER_FILE_SCHEDULER_H

#include <stdatomic.h>
#include <stdint.h>

#include "cleaner/core/fs_interface.h"
#include "cleaner/core/processor.h"
#include "cleaner/system/ws_scheduler.h"

typedef struct {
  processor_t *processor;
  ws_scheduler_t *executor;

  atomic_uint_fast64_t counter;
  atomic_uint_fast64_t pending_tasks;

} file_scheduler_t;

int file_scheduler_create(file_scheduler_t **out, processor_t *processor,
                          ws_scheduler_t *executor);

int file_scheduler_schedule(const fs_entry_t *entry, void *ctx);

int file_scheduler_run(file_scheduler_t *sched, const char *root_path);

void file_scheduler_wait(file_scheduler_t *sched);

void file_scheduler_destroy(file_scheduler_t *sched);

#endif