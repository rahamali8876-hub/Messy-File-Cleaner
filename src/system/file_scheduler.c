// src/system/file_scheduler.c

// #include "cleaner/system/file_scheduler.h"
// #include "cleaner/core/error.h"
// #include <stdlib.h>
// #include <string.h>
// #include <stdatomic.h>

// typedef struct
// {
//     file_scheduler_t *sched;
//     fs_entry_t entry;
// } task_ctx_t;

// static void task_fn(void *arg)
// {
//     task_ctx_t *ctx = arg;

//     uint64_t id =
//         atomic_fetch_add(&ctx->sched->counter, 1) + 1;

//     processor_process_file(ctx->sched->processor,
//                            &ctx->entry,
//                            id);

//     free((void *)ctx->entry.path);
//     free(ctx);
// }

// /* Directory callback MUST return int (fs interface constraint) */
// static int schedule_entry(const fs_entry_t *entry, void *user)
// {
//     file_scheduler_t *sched = user;

//     if (entry->is_directory)
//         return 0;

//     task_ctx_t *ctx = malloc(sizeof(task_ctx_t));
//     if (!ctx)
//         return -1;

//     ctx->sched = sched;
//     ctx->entry.path = strdup(entry->path);
//     ctx->entry.is_directory = entry->is_directory;

//     if (!ctx->entry.path)
//     {
//         free(ctx);
//         return -1;
//     }

//     error_t err =
//         sched->executor->submit(sched->executor,
//                                 task_fn,
//                                 ctx);

//     if (!error_is_ok(err))
//     {
//         free((void *)ctx->entry.path);
//         free(ctx);
//         return -1;
//     }

//     return 0;
// }

// error_t file_scheduler_run(file_scheduler_t *sched,
//                            const char *target_dir)
// {
//     if (!sched || !target_dir)
//         return error_make(ERR_INTERNAL,
//                           "Invalid scheduler parameters");

//     atomic_store(&sched->counter, 0);

//     int r =
//         sched->processor->fs.list_directory(
//             target_dir,
//             schedule_entry,
//             sched);

//     if (r != 0)
//         return error_make(ERR_WORK_FAILED,
//                           "Directory listing failed");

//     error_t wait_err =
//         sched->executor->wait_all(sched->executor);

//     if (!error_is_ok(wait_err))
//         return wait_err;

//     return error_ok();
// }
#include "cleaner/system/file_scheduler.h"
#include "cleaner/core/error.h"
#include <stdatomic.h>
#include <stdlib.h>
#include <string.h>

/* ========================================================= */
/* Task Context                                              */
/* ========================================================= */

typedef struct {
  file_scheduler_t *sched;
  fs_entry_t entry; /* FULL copy of entry */
} task_ctx_t;

/* ========================================================= */
/* Worker Task                                               */
/* ========================================================= */

static void task_fn(void *arg) {
  task_ctx_t *ctx = (task_ctx_t *)arg;

  uint64_t id = atomic_fetch_add(&ctx->sched->counter, 1) + 1;

  processor_process_file(ctx->sched->processor, &ctx->entry, id);

  /* Free deep-copied path */
  free((void *)ctx->entry.path);

  /* Free context */
  free(ctx);
}

/* ========================================================= */
/* Directory Callback (MUST return int)                     */
/* ========================================================= */

static int schedule_entry(const fs_entry_t *entry, void *user) {
  file_scheduler_t *sched = (file_scheduler_t *)user;

  if (!entry || entry->is_directory)
    return 0;

  task_ctx_t *ctx = (task_ctx_t *)malloc(sizeof(task_ctx_t));

  if (!ctx)
    return -1;

  ctx->sched = sched;

  /* ---- Copy FULL struct first ---- */
  ctx->entry = *entry;

  /* ---- Deep copy path ---- */
  if (entry->path) {
    ctx->entry.path = strdup(entry->path);
    if (!ctx->entry.path) {
      free(ctx);
      return -1;
    }
  } else {
    free(ctx);
    return -1;
  }

  /* Submit to executor */
  error_t err = sched->executor->submit(sched->executor, task_fn, ctx);

  if (!error_is_ok(err)) {
    free((void *)ctx->entry.path);
    free(ctx);
    return -1;
  }

  return 0;
}

/* ========================================================= */
/* Public API                                                */
/* ========================================================= */

error_t file_scheduler_run(file_scheduler_t *sched, const char *target_dir) {
  if (!sched || !target_dir)
    return error_make(ERR_INTERNAL, "Invalid scheduler parameters");

  atomic_store(&sched->counter, 0);

  int r =
      sched->processor->fs.list_directory(target_dir, schedule_entry, sched);

  if (r != 0)
    return error_make(ERR_WORK_FAILED, "Directory listing failed");

  error_t wait_err = sched->executor->wait_all(sched->executor);

  if (!error_is_ok(wait_err))
    return wait_err;

  return error_ok();
}