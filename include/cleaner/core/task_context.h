// include/cleaner/core/task_context.h

#ifndef CORE_TASK_CONTEXT_H
#define CORE_TASK_CONTEXT_H

#include <stdatomic.h>
#include <stdint.h>

typedef struct {
  uint64_t deadline_ms;   // absolute timestamp
  atomic_bool *cancelled; // shared cancel flag
} task_context_t;

// uint64_t now_ms();

#endif