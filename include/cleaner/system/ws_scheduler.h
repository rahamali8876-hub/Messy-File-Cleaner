// include/cleaner/system/ws_scheduler.h

#ifndef CLEANER_WS_SCHEDULER_H
#define CLEANER_WS_SCHEDULER_H

#include <stddef.h>

typedef void (*task_fn)(void *);

typedef struct ws_scheduler ws_scheduler_t;

ws_scheduler_t *ws_scheduler_create(int threads);
int ws_scheduler_submit(ws_scheduler_t *s, task_fn fn, void *arg);
void ws_scheduler_wait(ws_scheduler_t *s);
void ws_scheduler_destroy(ws_scheduler_t *s);

#endif