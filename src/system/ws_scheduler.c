// src/system/file_scheduler.c

#include "cleaner/system/ws_scheduler.h"

#include <pthread.h>
#include <stdatomic.h>
#include <stdlib.h>

#define WS_QUEUE_SIZE 1024

typedef struct
{
  task_fn fn;
  void *arg;
} task_t;

typedef struct
{
  atomic_size_t top;
  atomic_size_t bottom;
  task_t *tasks;
} deque_t;

typedef struct
{
  pthread_t thread;
  int id;
  struct ws_scheduler *sched;
  deque_t deque;
} worker_t;

typedef struct ws_scheduler
{
  int threads;
  atomic_int stop;

  worker_t *workers;

  atomic_long tasks_total;
  pthread_mutex_t wait_mutex;
  pthread_cond_t wait_cond;
} ws_scheduler_t;

static void deque_init(deque_t *d)
{
  atomic_store(&d->top, 0);
  atomic_store(&d->bottom, 0);
  d->tasks = calloc(WS_QUEUE_SIZE, sizeof(task_t));
}

static void deque_push(deque_t *d, task_t t)
{
  size_t b = atomic_load(&d->bottom);
  d->tasks[b % WS_QUEUE_SIZE] = t;
  atomic_thread_fence(memory_order_release);
  atomic_store(&d->bottom, b + 1);
}

static int deque_pop(deque_t *d, task_t *t)
{
  size_t b = atomic_load(&d->bottom) - 1;
  atomic_store(&d->bottom, b);

  atomic_thread_fence(memory_order_seq_cst);

  size_t top = atomic_load(&d->top);

  if (top <= b)
  {
    *t = d->tasks[b % WS_QUEUE_SIZE];
    if (top == b)
    {
      if (!atomic_compare_exchange_strong(&d->top, &top, top + 1))
        return 0;
      atomic_store(&d->bottom, b + 1);
    }
    return 1;
  }

  atomic_store(&d->bottom, b + 1);
  return 0;
}

static int deque_steal(deque_t *d, task_t *t)
{
  size_t top = atomic_load(&d->top);

  atomic_thread_fence(memory_order_seq_cst);

  size_t bottom = atomic_load(&d->bottom);

  if (top < bottom)
  {
    *t = d->tasks[top % WS_QUEUE_SIZE];
    if (atomic_compare_exchange_strong(&d->top, &top, top + 1))
      return 1;
  }

  return 0;
}

static void *worker_main(void *arg)
{
  worker_t *w = arg;
  ws_scheduler_t *s = w->sched;

  task_t task;

  while (!atomic_load(&s->stop))
  {

    if (deque_pop(&w->deque, &task))
    {
      task.fn(task.arg);

      if (atomic_fetch_sub(&s->tasks_total, 1) == 1)
      {
        pthread_mutex_lock(&s->wait_mutex);
        pthread_cond_signal(&s->wait_cond);
        pthread_mutex_unlock(&s->wait_mutex);
      }

      continue;
    }

    for (int i = 0; i < s->threads; i++)
    {
      if (i == w->id)
        continue;

      if (deque_steal(&s->workers[i].deque, &task))
      {
        task.fn(task.arg);

        if (atomic_fetch_sub(&s->tasks_total, 1) == 1)
        {
          pthread_mutex_lock(&s->wait_mutex);
          pthread_cond_signal(&s->wait_cond);
          pthread_mutex_unlock(&s->wait_mutex);
        }

        break;
      }
    }
  }

  return NULL;
}

ws_scheduler_t *ws_scheduler_create(int threads)
{
  if (threads <= 0)
    return NULL;

  ws_scheduler_t *s = calloc(1, sizeof(*s));
  if (!s)
    return NULL;

  s->threads = threads;
  s->workers = calloc((size_t)threads, sizeof(worker_t));
  if (!s->workers)
  {
    free(s);
    return NULL;
  }

  pthread_mutex_init(&s->wait_mutex, NULL);
  pthread_cond_init(&s->wait_cond, NULL);

  for (int i = 0; i < threads; i++)
  {

    worker_t *w = &s->workers[i];
    w->id = i;
    w->sched = s;

    deque_init(&w->deque);

    pthread_create(&w->thread, NULL, worker_main, w);
  }

  return s;
}

int ws_scheduler_submit(ws_scheduler_t *s, task_fn fn, void *arg)
{
  task_t t = {fn, arg};

  worker_t *w = &s->workers[rand() % s->threads];

  deque_push(&w->deque, t);

  atomic_fetch_add(&s->tasks_total, 1);

  return 0;
}

void ws_scheduler_wait(ws_scheduler_t *s)
{
  pthread_mutex_lock(&s->wait_mutex);

  while (atomic_load(&s->tasks_total) > 0)
    pthread_cond_wait(&s->wait_cond, &s->wait_mutex);

  pthread_mutex_unlock(&s->wait_mutex);
}

void ws_scheduler_destroy(ws_scheduler_t *s)
{
  atomic_store(&s->stop, 1);

  for (int i = 0; i < s->threads; i++)
    pthread_join(s->workers[i].thread, NULL);

  free(s->workers);
  free(s);
}