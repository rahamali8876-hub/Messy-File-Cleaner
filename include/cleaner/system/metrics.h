// cleaner/include\metrics.h

#ifndef CLEANER_METRICS_H
#define CLEANER_METRICS_H

#include <stdatomic.h>
#include <stdint.h>

typedef struct {
  atomic_uint_fast64_t files_processed;
  atomic_uint_fast64_t files_moved;
  atomic_uint_fast64_t errors;

  uint64_t start_ns;
  uint64_t end_ns;

  uint64_t (*now_ns)(void *ctx);
  void *time_ctx;

} metrics_t;

/* Lifecycle */
int metrics_init(metrics_t *m, uint64_t (*now_ns_fn)(void *), void *time_ctx);

void metrics_finish(metrics_t *m);

/* Counters */
void metrics_inc_processed(metrics_t *m);
void metrics_inc_moved(metrics_t *m);
void metrics_inc_error(metrics_t *m);

/* Getters */
uint64_t metrics_get_processed(metrics_t *m);
uint64_t metrics_get_moved(metrics_t *m);
uint64_t metrics_get_errors(metrics_t *m);

double metrics_get_elapsed_seconds(metrics_t *m);

#endif