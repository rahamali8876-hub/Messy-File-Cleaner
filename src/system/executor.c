// src/system/executor.c

#include "cleaner/system/executor.h"
#include <stdlib.h>
#include <stdatomic.h>

/* ============================= */
/* Internal Task Node            */
/* ============================= */

typedef struct executor_task
{
  executor_task_fn fn;
  void *arg;
  struct executor_task *next;
} executor_task_t;

/* ============================= */
/* Internal State                */
/* ============================= */

typedef struct executor_internal
{
  const cleaner_platform_api_t *platform;

  cleaner_threadpool_t *threadpool;

  atomic_uintptr_t task_head;
  atomic_int pending;
  atomic_int shutdown;

} executor_internal_t;

/* ============================= */
/* Worker                        */
/* ============================= */

static void executor_worker(void *arg)
{
  executor_internal_t *internal =
      (executor_internal_t *)arg;

  for (;;)
  {
    if (atomic_load(&internal->shutdown))
      return;

    executor_task_t *task =
        (executor_task_t *)atomic_load(
            &internal->task_head);

    if (!task)
      return;

    if (!atomic_compare_exchange_weak(
            &internal->task_head,
            (uintptr_t *)&task,
            (uintptr_t)task->next))
    {
      continue;
    }

    task->fn(task->arg);
    free(task);

    atomic_fetch_sub(&internal->pending, 1);
  }
}

/* ============================= */
/* Submit                        */
/* ============================= */

static error_t executor_submit(
    executor_interface_t *exec,
    executor_task_fn fn,
    void *arg)
{
  if (!exec || !exec->internal || !fn)
    return error_make(ERR_INTERNAL, "Invalid params");

  executor_internal_t *internal =
      (executor_internal_t *)exec->internal;

  if (atomic_load(&internal->shutdown))
    return error_make(ERR_INTERNAL, "Executor shutdown");

  executor_task_t *task =
      malloc(sizeof(*task));

  if (!task)
    return error_make(ERR_INTERNAL, "Alloc failed");

  task->fn = fn;
  task->arg = arg;

  executor_task_t *old_head;

  do
  {
    old_head = (executor_task_t *)
        atomic_load(&internal->task_head);

    task->next = old_head;

  } while (!atomic_compare_exchange_weak(
      &internal->task_head,
      (uintptr_t *)&old_head,
      (uintptr_t)task));

  atomic_fetch_add(&internal->pending, 1);

  internal->platform->threadpool_submit(
      internal->threadpool,
      executor_worker,
      internal);

  return error_ok();
}

/* ============================= */
/* Wait All                      */
/* ============================= */

static void executor_wait_all(
    executor_interface_t *exec)
{
  if (!exec || !exec->internal)
    return;

  executor_internal_t *internal =
      (executor_internal_t *)exec->internal;

  // while (atomic_load(&internal->pending) > 0)
  // {
  //   internal->platform->sleep_ms(1);
  // }

  // while (atomic_load(&internal->pending) > 0)
  // {
  //   /* spin */
  // }
  while (atomic_load(&internal->pending) > 0)
  {
#if defined(__GNUC__) || defined(__clang__)
    __asm__ __volatile__("pause");
#endif
  }
}

/* ============================= */
/* Create                        */
/* ============================= */

error_t executor_create(
    const cleaner_platform_api_t *platform,
    int threads,
    executor_interface_t **out_exec)
{
  if (!platform || threads <= 0 || !out_exec)
    return error_make(ERR_INTERNAL, "Invalid params");

  executor_internal_t *internal =
      calloc(1, sizeof(*internal));

  if (!internal)
    return error_make(ERR_INTERNAL, "Alloc failed");

  internal->platform = platform;

  atomic_store(&internal->task_head, 0);
  atomic_store(&internal->pending, 0);
  atomic_store(&internal->shutdown, 0);

  internal->threadpool =
      platform->threadpool_create(threads, 0);

  if (!internal->threadpool)
    return error_make(ERR_INTERNAL, "Threadpool failed");

  executor_interface_t *iface =
      malloc(sizeof(*iface));

  if (!iface)
    return error_make(ERR_INTERNAL, "Alloc failed");

  iface->internal = internal;
  iface->submit = executor_submit;
  iface->wait_all = executor_wait_all;

  *out_exec = iface;

  return error_ok();
}

/* ============================= */
/* Destroy                       */
/* ============================= */

void executor_destroy(
    executor_interface_t *exec)
{
  if (!exec || !exec->internal)
    return;

  executor_internal_t *internal =
      (executor_internal_t *)exec->internal;

  atomic_store(&internal->shutdown, 1);

  internal->platform->threadpool_destroy(
      internal->threadpool);

  free(internal);
  free(exec);
}