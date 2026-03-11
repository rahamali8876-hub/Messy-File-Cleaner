// cleaner/src/scanner.c

#include "cleaner/system/scanner.h"

#include <stdatomic.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PATH 512
#define DIR_QUEUE 8192

typedef struct
{
  char path[MAX_PATH];

} dir_node_t;

typedef struct
{
  processor_t *processor;
  mpmc_executor_t *executor;

  dir_node_t *queue;

  atomic_size_t head;
  atomic_size_t tail;

  atomic_uint_fast64_t id;

} scanner_ctx_t;

/* ================= FILE TASK ================= */

typedef struct
{
  processor_t *processor;
  fs_entry_t entry;
  uint64_t id;

} file_task_t;

static void file_worker(void *arg)
{
  file_task_t *task = arg;

  processor_process_file(task->processor, &task->entry, task->id);

  free(task);
}

/* ================= DIRECTORY CALLBACK ================= */

static int scan_callback(const fs_entry_t *entry, void *arg)
{
  scanner_ctx_t *ctx = arg;

  if (!entry)
    return 0;

  /* submit file processing */

  if (!entry->is_directory)
  {
    file_task_t *task = malloc(sizeof(*task));
    if (!task)
      return -1;

    task->processor = ctx->processor;
    task->entry = *entry;
    task->id = atomic_fetch_add(&ctx->id, 1);

    mpmc_executor_submit(ctx->executor, file_worker, task);
  }
  else
  {
    size_t t = atomic_fetch_add(&ctx->tail, 1);

    if (t >= DIR_QUEUE)
      return -1;

    strncpy(ctx->queue[t].path, entry->path, MAX_PATH - 1);
    ctx->queue[t].path[MAX_PATH - 1] = '\0';
  }

  return 0;
}

/* ================= SCAN WORKER ================= */

static void scan_worker(void *arg)
{
  scanner_ctx_t *ctx = arg;

  for (;;)
  {
    size_t h = atomic_fetch_add(&ctx->head, 1);

    if (h >= atomic_load(&ctx->tail))
      break;

    dir_node_t *node = &ctx->queue[h];

    ctx->processor->fs.list_directory(ctx->processor->fs.context, node->path,
                                      scan_callback, ctx);
  }
}

/* ================= PUBLIC API ================= */

error_t scanner_run(processor_t *processor, const char *root,
                    mpmc_executor_t *executor, int scan_threads)
{
  if (!processor || !root || !executor || scan_threads <= 0)
    return error_make(ERR_INTERNAL, "Invalid scanner parameters");

  scanner_ctx_t ctx;

  ctx.processor = processor;
  ctx.executor = executor;

  ctx.queue = calloc(DIR_QUEUE, sizeof(dir_node_t));
  if (!ctx.queue)
    return error_make(ERR_INTERNAL, "Queue alloc failed");

  atomic_init(&ctx.head, 0);
  atomic_init(&ctx.tail, 1);
  atomic_init(&ctx.id, 1);

  strncpy(ctx.queue[0].path, root, MAX_PATH - 1);
  ctx.queue[0].path[MAX_PATH - 1] = '\0';

  /* submit scan workers to executor */

  for (int i = 0; i < scan_threads; i++)
  {
    mpmc_executor_submit(executor, scan_worker, &ctx);
  }

  /* wait until scanning done */

  while (atomic_load(&ctx.head) < atomic_load(&ctx.tail))
  {
    /* simple wait loop */
  }

  free(ctx.queue);

  return error_ok();
}
