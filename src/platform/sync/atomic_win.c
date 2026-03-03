// 📄 src/platform/sync/atomic_win.c

/* INTERNAL - DO NOT INCLUDE OUTSIDE PLATFORM */

#include "cleaner/platform/sync/atomic_win.h"
#include <windows.h>

void cleaner_atomic_init(cleaner_atomic_i64 *a, int64_t v) { a->value = v; }

int64_t cleaner_atomic_increment(cleaner_atomic_i64 *a) {
  return InterlockedIncrement64(&a->value);
}

int64_t cleaner_atomic_load(const cleaner_atomic_i64 *a) { return a->value; }