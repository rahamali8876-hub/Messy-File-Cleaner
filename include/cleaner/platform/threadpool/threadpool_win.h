
// cleaner/platform/threadpool_win.h

#ifndef CLEANER_THREADPOOL_WIN_H
#define CLEANER_THREADPOOL_WIN_H

#include <stddef.h>
#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*task_fn)(void *);

/* Opaque type — implementation owns layout */
typedef struct threadpool threadpool_t;

/* API */

int threadpool_init(threadpool_t **tp, size_t threads, size_t queue_capacity);
void threadpool_submit(threadpool_t *tp, task_fn fn, void *arg);
void threadpool_wait_all(threadpool_t *tp);
void threadpool_shutdown(threadpool_t *tp);

#ifdef __cplusplus
}
#endif

#endif