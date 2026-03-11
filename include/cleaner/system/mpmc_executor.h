#ifndef CLEANER_SYSTEM_MPMC_EXECUTOR_H
#define CLEANER_SYSTEM_MPMC_EXECUTOR_H

#include "cleaner/core/error.h"
#include "cleaner/platform/platform_api.h"
#include <stdatomic.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  void (*fn)(void *);
  void *arg;

} mpmc_task_t;

typedef struct {
  atomic_size_t seq;
  mpmc_task_t task;

} mpmc_cell_t;

typedef struct {
  const cleaner_platform_api_t *platform;

  size_t capacity;
  size_t mask;

  mpmc_cell_t *buffer;

  atomic_size_t enqueue_pos;
  atomic_size_t dequeue_pos;

  atomic_int stop;

  cleaner_threadpool_t *workers;

} mpmc_executor_t;

error_t mpmc_executor_create(const cleaner_platform_api_t *platform,
                             int threads, size_t capacity,
                             mpmc_executor_t **out);

error_t mpmc_executor_submit(mpmc_executor_t *exec, void (*fn)(void *),
                             void *arg);

void mpmc_executor_destroy(mpmc_executor_t *exec);

#ifdef __cplusplus
}
#endif

#endif