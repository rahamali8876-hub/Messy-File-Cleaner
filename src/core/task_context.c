// src/core/task_context.c

#include "cleaner/core/task_context.h"
#include <time.h>

uint64_t now_ms() {
  struct timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  return (uint64_t)ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}