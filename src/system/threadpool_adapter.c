// src/system/threadpool_adaptar.c

#include "cleaner/system/threadpool_adapter.h"
#include "cleaner/platform/threadpool/threadpool_win.h"

static threadpool_t *g_tp = NULL;

static void submit_adapter(executor_task_fn fn, void *arg)
{
  threadpool_submit(g_tp, fn, arg);
}

static void wait_adapter(void) { threadpool_wait_all(g_tp); }

static void shutdown_adapter(void) { threadpool_shutdown(g_tp); }

int build_threadpool_executor(executor_interface_t *out, int thread_count)
{
  if (threadpool_init(&g_tp, (size_t)thread_count, (size_t)1024) != 0)
    return -1;

  out->submit = submit_adapter;
  out->wait_all = wait_adapter;
  out->shutdown = shutdown_adapter;

  return 0;
}

void destroy_threadpool_executor(void)
{
  if (g_tp)
    threadpool_shutdown(g_tp);
}