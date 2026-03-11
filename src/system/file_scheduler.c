// src/system/file_scheduler.c

#include "cleaner/system/file_scheduler.h"
#include "cleaner/system/ws_scheduler.h"

#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#define thread_yield() Sleep(0)
#else
#include <sched.h>
#define thread_yield() sched_yield()
#endif

/* ============================= */
/* Task Context */
/* ============================= */

typedef struct
{
  file_scheduler_t *sched;
  fs_entry_t entry;

} task_ctx_t;

/* ============================= */
/* Worker Task */
/* ============================= */

static void process_task(void *arg)
{
  task_ctx_t *ctx = arg;

  uint64_t id =
      atomic_fetch_add_explicit(&ctx->sched->counter, 1, memory_order_relaxed);

  processor_process_file(ctx->sched->processor, &ctx->entry, id);

  // Save scheduler pointer before freeing ctx
  file_scheduler_t *sched = ctx->sched;

  free((void *)ctx->entry.path);
  free(ctx);

  atomic_fetch_sub_explicit(&sched->pending_tasks, 1, memory_order_release);
}

/* ============================= */
/* Create Scheduler */
/* ============================= */

int file_scheduler_create(file_scheduler_t **out, processor_t *processor,
                          ws_scheduler_t *executor)
{
  file_scheduler_t *sched = calloc(1, sizeof(*sched));
  if (!sched)
    return -1;

  sched->processor = processor;
  sched->executor = executor;

  atomic_init(&sched->counter, 0);
  atomic_init(&sched->pending_tasks, 0);

  *out = sched;

  return 0;
}

/* ============================= */
/* Schedule Entry */
/* ============================= */

int file_scheduler_schedule(const fs_entry_t *entry, void *ctx)
{
  file_scheduler_t *sched = ctx;

  if (!entry || !sched)
    return 0;

  if (entry->is_directory)
    return 0;

  task_ctx_t *task = malloc(sizeof(*task));
  if (!task)
    return -1;

  task->sched = sched;
  task->entry = *entry;

  task->entry.path = strdup(entry->path);
  if (!task->entry.path)
  {
    free(task);
    return -1;
  }

  atomic_fetch_add_explicit(&sched->pending_tasks, 1, memory_order_relaxed);

  if (ws_scheduler_submit(sched->executor, process_task, task) != 0)
  {
    free((void *)task->entry.path);
    free(task);

    atomic_fetch_sub_explicit(&sched->pending_tasks, 1, memory_order_relaxed);

    return -1;
  }

  return 0;
}

/* ============================= */
/* Wait For Tasks */
/* ============================= */

void file_scheduler_wait(file_scheduler_t *sched)
{
  for (;;)
  {
    if (atomic_load_explicit(&sched->pending_tasks, memory_order_acquire) == 0)
      break;

    thread_yield();
  }
}

/* ============================= */
/* Destroy */
/* ============================= */

void file_scheduler_destroy(file_scheduler_t *sched)
{
  if (!sched)
    return;

  free(sched);
}

/* ============================= */
/* Run Scheduler */
/* ============================= */

/**
 * Main scheduler loop: scan directories and process files
 */
int file_scheduler_run(file_scheduler_t *sched, const char *root_path)
{
  if (!sched || !sched->processor || !root_path)
    return -1;

  fs_interface_t *fs = &sched->processor->fs;
  if (!fs || !fs->list_directory)
    return -1;

  printf("[Scheduler] Starting scan from: %s\n", root_path);

  // Recursively list and process files
  fs->list_directory(fs->context, root_path, file_scheduler_schedule, sched);

  // Wait for all tasks to complete
  file_scheduler_wait(sched);

  printf("[Scheduler] Scan complete. Processed %llu files\n",
         (unsigned long long)atomic_load(&sched->counter));

  return 0;
}