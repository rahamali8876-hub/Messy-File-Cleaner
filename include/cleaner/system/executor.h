// cleaner/include/cleaner/system/executor.h

#ifndef CLEANER_SYSTEM_EXECUTOR_H
#define CLEANER_SYSTEM_EXECUTOR_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stddef.h>
#include "cleaner/platform/platform_api.h"
#include "cleaner/core/error.h"

  /* Task function signature */
  typedef void (*executor_task_fn)(void *arg);

  /* Public interface */
  typedef struct executor_interface
  {
    void *internal;

    error_t (*submit)(
        struct executor_interface *exec,
        executor_task_fn fn,
        void *arg);

    void (*wait_all)(
        struct executor_interface *exec);

  } executor_interface_t;

  /* Create executor */
  error_t executor_create(
      const cleaner_platform_api_t *platform,
      int threads,
      executor_interface_t **out_exec);

  /* Destroy executor */
  void executor_destroy(
      executor_interface_t *exec);

#ifdef __cplusplus
}
#endif

#endif