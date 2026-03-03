// cleaner/src/system/metrics.c

#include "cleaner/system/metrics.h"
#include <string.h>

int metrics_init(metrics_t *m, uint64_t (*now_ns_fn)(void *), void *time_ctx) {
  if (!m || !now_ns_fn)
    return -1;

  memset(m, 0, sizeof(*m));

  m->now_ns = now_ns_fn;
  m->time_ctx = time_ctx;
  m->start_ns = now_ns_fn(time_ctx);

  return 0;
}

void metrics_finish(metrics_t *m) {
  if (!m || !m->now_ns)
    return;

  m->end_ns = m->now_ns(m->time_ctx);
}

void metrics_inc_processed(metrics_t *m) {
  atomic_fetch_add(&m->files_processed, 1);
}

void metrics_inc_moved(metrics_t *m) { atomic_fetch_add(&m->files_moved, 1); }

void metrics_inc_error(metrics_t *m) { atomic_fetch_add(&m->errors, 1); }

uint64_t metrics_get_processed(metrics_t *m) {
  return atomic_load(&m->files_processed);
}

uint64_t metrics_get_moved(metrics_t *m) {
  return atomic_load(&m->files_moved);
}

uint64_t metrics_get_errors(metrics_t *m) { return atomic_load(&m->errors); }

double metrics_get_elapsed_seconds(metrics_t *m) {
  uint64_t end = m->end_ns ? m->end_ns : m->now_ns(m->time_ctx);
  return (double)(end - m->start_ns) / 1e9;
}