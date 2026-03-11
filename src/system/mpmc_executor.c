// src/system/mpmp_executor.c

#include "cleaner/system/mpmc_executor.h"

#include <stdlib.h>

/* ================= Worker ================= */

static void worker_loop(void *arg)
{
  mpmc_executor_t *exec = arg;

  for (;;)
  {
    if (atomic_load(&exec->stop))
      return;

    size_t pos = atomic_load(&exec->dequeue_pos);

    mpmc_cell_t *cell = &exec->buffer[pos & exec->mask];

    size_t seq = atomic_load(&cell->seq);

    intptr_t dif = (intptr_t)seq - (intptr_t)(pos + 1);

    if (dif == 0)
    {
      if (atomic_compare_exchange_weak(&exec->dequeue_pos, &pos, pos + 1))
      {
        mpmc_task_t task = cell->task;

        atomic_store(&cell->seq, pos + exec->mask + 1);

        task.fn(task.arg);
      }
    }
  }
}

/* ================= Create ================= */

error_t mpmc_executor_create(const cleaner_platform_api_t *platform,
                             int threads, size_t capacity,
                             mpmc_executor_t **out)
{
  if (!platform || !out || threads <= 0)
    return error_make(ERR_INTERNAL, "Invalid executor params");

  if ((capacity & (capacity - 1)) != 0)
    return error_make(ERR_INTERNAL, "Capacity must be power of 2");

  mpmc_executor_t *exec = calloc(1, sizeof(*exec));
  if (!exec)
    return error_make(ERR_INTERNAL, "Alloc failed");

  exec->capacity = capacity;
  exec->mask = capacity - 1;
  exec->platform = platform;

  exec->buffer = calloc(capacity, sizeof(mpmc_cell_t));
  if (!exec->buffer)
  {
    free(exec);
    return error_make(ERR_INTERNAL, "Buffer alloc failed");
  }

  for (size_t i = 0; i < capacity; i++)
    atomic_store(&exec->buffer[i].seq, i);

  atomic_store(&exec->enqueue_pos, 0);
  atomic_store(&exec->dequeue_pos, 0);
  atomic_store(&exec->stop, 0);

  exec->workers = platform->threadpool_create(threads, 0);

  if (!exec->workers)
  {
    free(exec->buffer);
    free(exec);
    return error_make(ERR_INTERNAL, "Threadpool failed");
  }

  for (int i = 0; i < threads; i++)
    platform->threadpool_submit(exec->workers, worker_loop, exec);

  *out = exec;

  return error_ok();
}

/* ================= Submit ================= */

error_t mpmc_executor_submit(mpmc_executor_t *exec, void (*fn)(void *),
                             void *arg)
{
  if (!exec || !fn)
    return error_make(ERR_INTERNAL, "Invalid task");

  mpmc_cell_t *cell;
  size_t pos = atomic_load(&exec->enqueue_pos);

  for (;;)
  {
    cell = &exec->buffer[pos & exec->mask];

    size_t seq = atomic_load(&cell->seq);

    intptr_t dif = (intptr_t)seq - (intptr_t)pos;

    if (dif == 0)
    {
      if (atomic_compare_exchange_weak(&exec->enqueue_pos, &pos, pos + 1))
        break;
    }
    else if (dif < 0)
    {
      return error_make(ERR_QUEUE_FULL, "Executor queue full");
    }
    else
    {
      pos = atomic_load(&exec->enqueue_pos);
    }
  }

  cell->task.fn = fn;
  cell->task.arg = arg;

  atomic_store(&cell->seq, pos + 1);

  return error_ok();
}

/* ================= Destroy ================= */

void mpmc_executor_destroy(mpmc_executor_t *exec)
{
  if (!exec)
    return;

  atomic_store(&exec->stop, 1);

  if (exec->workers)
    exec->platform->threadpool_destroy(exec->workers);

  free(exec->buffer);
  free(exec);
}