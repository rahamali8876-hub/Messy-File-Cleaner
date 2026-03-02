// include/cleaner/system/file_scheduler.h

#ifndef CLEANER_SYSTEM_FILE_SCHEDULER_H
#define CLEANER_SYSTEM_FILE_SCHEDULER_H

#include <stdatomic.h>
#include "cleaner/core/error.h"
#include "cleaner/core/processor.h"
#include "cleaner/system/executor.h"

typedef struct
{
    processor_t *processor;
    executor_interface_t *executor;
    atomic_uint_fast64_t counter;
} file_scheduler_t;

/* Run scheduler on target directory */

error_t file_scheduler_run(file_scheduler_t *sched,
                           const char *target_dir);

#endif