// // src/core/scanner_parallel.c

// #include "cleaner/core/scanner_parallel.h"
// #include "cleaner/core/work_queue.h"

// #include <stdio.h>
// #include <string.h>

// typedef struct
// {
//     processor_t *processor;
//     work_queue_t *queue;
// } worker_ctx_t;

// static int scan_callback(const fs_entry_t *entry, void *ctx)
// {
//     worker_ctx_t *w = (worker_ctx_t *)ctx;

//     static unsigned long long id = 1;

//     processor_process_file(w->processor, entry, id++);

//     if (entry->is_directory)
//     {
//         work_queue_push(w->queue, entry->path);
//     }

//     return 0;
// }

// static void worker_thread(void *arg)
// {
//     worker_ctx_t *ctx = (worker_ctx_t *)arg;

//     char path[512];

//     while (1)
//     {
//         if (work_queue_pop(ctx->queue, path) != 0)
//             break;

//         ctx->processor->fs.list_directory(
//             ctx->processor->fs.context,
//             path,
//             scan_callback,
//             ctx);
//     }
// }

// #include "cleaner/core/scanner_parallel.h"
// #include "cleaner/core/processor.h"

// void scanner_parallel_run(
//     cleaner_processor_t *processor,
//     const char *path,
//     int threads
// )
// {
//     cleaner_threadpool_t *pool =
//         processor->platform->threadpool_create(threads, 0);

//     for(int i = 0; i < 10; i++)
//     {
//         processor->platform->threadpool_submit(
//             pool,
//             scan_task,
//             NULL
//         );
//     }

//     processor->platform->threadpool_destroy(pool);
// }

// int scanner_parallel_run(processor_t *processor,
//                          const char *root,
//                          int threads)
// {
//     if (!processor || !root)
//         return -1;

//     work_queue_t queue;

//     work_queue_init(&queue);

//     work_queue_push(&queue, root);

//     cleaner_threadpool_t *pool =
//         processor->fs.context->platform->threadpool_create(threads, 0);

//     worker_ctx_t ctx;

//     ctx.processor = processor;
//     ctx.queue = &queue;

//     for (int i = 0; i < threads; i++)
//     {
//         processor->fs.context->platform->threadpool_submit(
//             pool,
//             worker_thread,
//             &ctx);
//     }

//     processor->fs.context->platform->threadpool_destroy(pool);

//     return 0;
// }

#include "cleaner/core/scanner_parallel.h"
#include "cleaner/core/processor.h"
#include "cleaner/core/work_queue.h"
#include "cleaner/platform/platform_api.h"

#include <stdio.h>
#include <string.h>

typedef struct {
  processor_t *processor;
  work_queue_t *queue;

} worker_ctx_t;

static int scan_callback(const fs_entry_t *entry, void *ctx) {
  worker_ctx_t *w = (worker_ctx_t *)ctx;

  static unsigned long long id = 1;

  processor_process_file(w->processor, entry, id++);

  if (entry->is_directory) {
    work_queue_push(w->queue, entry->path);
  }

  return 0;
}

static void worker_thread(void *arg) {
  worker_ctx_t *ctx = (worker_ctx_t *)arg;

  char path[512];

  while (1) {
    if (work_queue_pop(ctx->queue, path) != 0)
      break;

    ctx->processor->fs.list_directory(ctx->processor->fs.context, path,
                                      scan_callback, ctx);
  }
}

int scanner_parallel_run(processor_t *processor, const char *root,
                         int threads) {
  if (!processor || !root)
    return -1;

  work_queue_t queue;

  work_queue_init(&queue);

  work_queue_push(&queue, root);

  const cleaner_platform_api_t *platform = cleaner_platform_get_api();

  cleaner_threadpool_t *pool = platform->threadpool_create(threads, 0);

  worker_ctx_t ctx;

  ctx.processor = processor;
  ctx.queue = &queue;

  for (int i = 0; i < threads; i++) {
    platform->threadpool_submit(pool, worker_thread, &ctx);
  }

  platform->threadpool_destroy(pool);

  return 0;
}