
// 📄 src/platform/time_posix.c

#include <stdint.h>
#include <time.h>

#include "cleaner/platform/time.h"

uint64_t cleaner_time_now_ns(void) {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);

  return (uint64_t)ts.tv_sec * 1000000000ULL + (uint64_t)ts.tv_nsec;
}