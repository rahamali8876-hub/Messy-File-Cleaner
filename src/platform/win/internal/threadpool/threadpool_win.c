
// src/platform/win/internal/threadpool/threadpool_win.c

#include "cleaner/platform/threadpool/threadpool_win.h"
#include <stdlib.h>
#include <windows.h>

/* ===================================================== */
/* Internal Structures                                   */
/* ===================================================== */

typedef struct task_node {
  task_fn fn;
  void *arg;
  struct task_node *next;
} task_node_t;

struct threadpool {
  HANDLE *threads;
  int thread_count;
  int queue_capacity;

  task_node_t *head;
  task_node_t *tail;
  int queue_size;

  int active_count; /* running tasks */

  CRITICAL_SECTION lock;
  CONDITION_VARIABLE cond;      /* work available */
  CONDITION_VARIABLE idle_cond; /* for wait_all */

  int stop;
};

/* ===================================================== */
/* Worker Thread                                         */
/* ===================================================== */

static DWORD WINAPI thread_worker(LPVOID lpParam) {
  threadpool_t *tp = (threadpool_t *)lpParam;

  while (1) {
    EnterCriticalSection(&tp->lock);

    while (!tp->head && !tp->stop)
      SleepConditionVariableCS(&tp->cond, &tp->lock, INFINITE);

    if (tp->stop && !tp->head) {
      LeaveCriticalSection(&tp->lock);
      break;
    }

    task_node_t *task = tp->head;

    if (task) {
      tp->head = task->next;
      if (!tp->head)
        tp->tail = NULL;

      tp->queue_size--;
      tp->active_count++;
    }

    LeaveCriticalSection(&tp->lock);

    if (task) {
      task->fn(task->arg);
      free(task);

      EnterCriticalSection(&tp->lock);
      tp->active_count--;

      if (tp->queue_size == 0 && tp->active_count == 0)
        WakeAllConditionVariable(&tp->idle_cond);

      LeaveCriticalSection(&tp->lock);
    }
  }

  return 0;
}

// static DWORD WINAPI thread_worker(LPVOID lpParam)
// {
//     threadpool_t *tp = (threadpool_t *)lpParam;

//     while (1)
//     {
//         EnterCriticalSection(&tp->lock);

//         while (!tp->head && !tp->stop)
//             SleepConditionVariableCS(&tp->cond, &tp->lock, INFINITE);

//         if (tp->stop)
//         {
//             LeaveCriticalSection(&tp->lock);
//             break;
//         }

//         task_node_t *task = tp->head;

//         if (task)
//         {
//             tp->head = task->next;
//             if (!tp->head)
//                 tp->tail = NULL;

//             tp->queue_size--;
//             tp->active_count++;
//         }

//         LeaveCriticalSection(&tp->lock);

//         if (task)
//         {
//             task->fn(task->arg);
//             free(task);

//             EnterCriticalSection(&tp->lock);
//             tp->active_count--;

//             if (tp->queue_size == 0 && tp->active_count == 0)
//                 WakeAllConditionVariable(&tp->idle_cond);

//             LeaveCriticalSection(&tp->lock);
//         }
//     }

//     return 0;
// }

/* ===================================================== */
/* Init                                                  */
/* ===================================================== */

int threadpool_init(threadpool_t **out, int threads, int queue_capacity) {
  threadpool_t *tp = malloc(sizeof(threadpool_t));
  if (!tp)
    return -1;

  tp->threads = malloc(sizeof(HANDLE) * threads);
  if (!tp->threads) {
    free(tp);
    return -1;
  }

  tp->thread_count = threads;
  tp->queue_capacity = queue_capacity;
  tp->head = tp->tail = NULL;
  tp->queue_size = 0;
  tp->active_count = 0;
  tp->stop = 0;

  InitializeCriticalSection(&tp->lock);
  InitializeConditionVariable(&tp->cond);
  InitializeConditionVariable(&tp->idle_cond);

  for (int i = 0; i < threads; i++) {
    tp->threads[i] = CreateThread(NULL, 0, thread_worker, tp, 0, NULL);

    if (!tp->threads[i])
      return -1;
  }

  *out = tp;
  return 0;
}

/* ===================================================== */
/* Submit                                                */
/* ===================================================== */

void threadpool_submit(threadpool_t *tp, task_fn fn, void *arg) {
  task_node_t *node = malloc(sizeof(task_node_t));
  if (!node)
    return;

  node->fn = fn;
  node->arg = arg;
  node->next = NULL;

  EnterCriticalSection(&tp->lock);

  if (tp->tail)
    tp->tail->next = node;
  else
    tp->head = node;

  tp->tail = node;
  tp->queue_size++;

  WakeConditionVariable(&tp->cond);

  LeaveCriticalSection(&tp->lock);
}

/* ===================================================== */
/* Proper Blocking Wait                                  */
/* ===================================================== */

void threadpool_wait_all(threadpool_t *tp) {
  EnterCriticalSection(&tp->lock);

  while (tp->queue_size > 0 || tp->active_count > 0)
    SleepConditionVariableCS(&tp->idle_cond, &tp->lock, INFINITE);

  LeaveCriticalSection(&tp->lock);
}

/* ===================================================== */
/* Shutdown                                              */
/* ===================================================== */

void threadpool_shutdown(threadpool_t *tp) {
  if (!tp)
    return;

  /* FIRST: wait for all tasks to complete */
  threadpool_wait_all(tp);

  /* THEN: signal workers to stop */
  EnterCriticalSection(&tp->lock);
  tp->stop = 1;
  WakeAllConditionVariable(&tp->cond);
  LeaveCriticalSection(&tp->lock);

  /* Join threads */
  for (int i = 0; i < tp->thread_count; i++) {
    WaitForSingleObject(tp->threads[i], INFINITE);
    CloseHandle(tp->threads[i]);
  }

  DeleteCriticalSection(&tp->lock);
  free(tp->threads);

  /* Free remaining queue nodes (should be none) */
  while (tp->head) {
    task_node_t *tmp = tp->head;
    tp->head = tmp->next;
    free(tmp);
  }

  free(tp);
}

// void threadpool_shutdown(threadpool_t *tp)
// {
//     EnterCriticalSection(&tp->lock);
//     tp->stop = 1;
//     WakeAllConditionVariable(&tp->cond);
//     LeaveCriticalSection(&tp->lock);

//     for (int i = 0; i < tp->thread_count; i++)
//     {
//         WaitForSingleObject(tp->threads[i], INFINITE);
//         CloseHandle(tp->threads[i]);
//     }

//     DeleteCriticalSection(&tp->lock);
//     free(tp->threads);

//     while (tp->head)
//     {
//         task_node_t *tmp = tp->head;
//         tp->head = tmp->next;
//         free(tmp);
//     }

//     free(tp);
// }