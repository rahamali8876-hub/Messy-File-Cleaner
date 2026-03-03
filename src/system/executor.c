// src/system/executor.c

#include "cleaner/system/executor.h"
#include <stdatomic.h>
#include <stdlib.h>

/* ========================================================= */
/* Internal Structures                                       */
/* ========================================================= */

typedef struct {
  cleaner_threadpool_t *pool;
  atomic_int pending;
  const cleaner_platform_api_t *platform;
} executor_internal_t;

/* Wrapper context for submitted tasks */
typedef struct {
  void (*fn)(void *);
  void *arg;
  atomic_int *pending_counter;
} executor_task_wrapper_t;

/* ========================================================= */
/* Task Wrapper                                              */
/* ========================================================= */

static void executor_task_wrapper(void *arg) {
  executor_task_wrapper_t *ctx = (executor_task_wrapper_t *)arg;

  /* Execute real task */
  ctx->fn(ctx->arg);

  /* Decrement pending counter */
  atomic_fetch_sub(ctx->pending_counter, 1);

  /* Free wrapper */
  free(ctx);
}

/* ========================================================= */
/* Submit                                                    */
/* ========================================================= */

static error_t executor_submit(executor_interface_t *exec, void (*fn)(void *),
                               void *arg) {
  if (!exec || !exec->internal || !fn)
    return error_make(ERR_INTERNAL, "Executor not initialized");

  executor_internal_t *internal = (executor_internal_t *)exec->internal;

  executor_task_wrapper_t *ctx = malloc(sizeof(executor_task_wrapper_t));

  if (!ctx)
    return error_make(ERR_INTERNAL, "Task allocation failed");

  ctx->fn = fn;
  ctx->arg = arg;
  ctx->pending_counter = &internal->pending;

  /* Increase pending BEFORE submission */
  atomic_fetch_add(&internal->pending, 1);

  internal->platform->threadpool_submit(internal->pool, executor_task_wrapper,
                                        ctx);

  return error_ok();
}

/* ========================================================= */
/* Wait All                                                  */
/* ========================================================= */

static error_t executor_wait_all(executor_interface_t *exec) {
  if (!exec || !exec->internal)
    return error_make(ERR_INTERNAL, "Executor not initialized");

  executor_internal_t *internal = (executor_internal_t *)exec->internal;

  /* Busy wait (platform has no condvar API) */
  while (atomic_load(&internal->pending) > 0) {
    internal->platform->time_now(NULL); /* yield */
  }

  return error_ok();
}

/* ========================================================= */
/* Create                                                    */
/* ========================================================= */

error_t executor_create(const cleaner_platform_api_t *platform, int threads,
                        executor_interface_t **out_exec) {
  if (!platform || threads <= 0 || !out_exec)
    return error_make(ERR_INTERNAL, "Invalid executor parameters");

  executor_internal_t *internal = malloc(sizeof(executor_internal_t));

  if (!internal)
    return error_make(ERR_INTERNAL, "Internal allocation failed");

  internal->pool = platform->threadpool_create(threads, 0);

  if (!internal->pool) {
    free(internal);
    return error_make(ERR_INTERNAL, "Threadpool creation failed");
  }

  internal->platform = platform;
  atomic_store(&internal->pending, 0);

  executor_interface_t *iface = malloc(sizeof(executor_interface_t));

  if (!iface) {
    free(internal);
    return error_make(ERR_INTERNAL, "Interface allocation failed");
  }

  iface->internal = internal;
  iface->submit = executor_submit;
  iface->wait_all = executor_wait_all;

  *out_exec = iface;

  return error_ok();
}

/* ========================================================= */
/* Destroy                                                   */
/* ========================================================= */

void executor_destroy(executor_interface_t *exec) {
  if (!exec || !exec->internal)
    return;

  executor_internal_t *internal = (executor_internal_t *)exec->internal;

  /*
      IMPORTANT:
      If your platform API provides threadpool_destroy(),
      call it here before freeing internal.
  */

  free(internal);
  free(exec);
}