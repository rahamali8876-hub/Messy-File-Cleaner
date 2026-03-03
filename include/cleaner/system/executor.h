// // // cleaner/include/cleaner/system/executor.h

#ifndef CLEANER_SYSTEM_EXECUTOR_H
#define CLEANER_SYSTEM_EXECUTOR_H

#include "cleaner/core/error.h"
#include "cleaner/platform/platform_api.h"

typedef struct executor_interface executor_interface_t;

struct executor_interface {
  void *internal;

  error_t (*submit)(executor_interface_t *, void (*fn)(void *), void *arg);
  error_t (*wait_all)(executor_interface_t *);
};

/* Create executor */
error_t executor_create(const cleaner_platform_api_t *platform, int threads,
                        executor_interface_t **out_exec);

/* Destroy executor */
void executor_destroy(executor_interface_t *executor);

#endif