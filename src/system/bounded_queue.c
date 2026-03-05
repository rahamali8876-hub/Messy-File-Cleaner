// src/system/bounded_queue.c

#include "cleaner/system/bounded_queue.h"
#include <stdlib.h>

int bq_init(bounded_queue_t *q,
            const cleaner_platform_api_t *api,
            size_t capacity)
{
  if (!q || !api || capacity == 0)
    return -1;

  q->buffer = malloc(capacity * sizeof(*q->buffer));
  if (!q->buffer)
    return -1;

  q->capacity = capacity;
  q->head = 0;
  q->tail = 0;
  q->size = 0;

  q->mutex = api->mutex_create();
  q->not_empty = api->cond_create();
  q->not_full = api->cond_create();

  if (!q->mutex || !q->not_empty || !q->not_full)
  {
    if (q->mutex)
      api->mutex_destroy(q->mutex);
    if (q->not_empty)
      api->cond_destroy(q->not_empty);
    if (q->not_full)
      api->cond_destroy(q->not_full);
    free(q->buffer);
    return -1;
  }

  return 0;
}

void bq_destroy(bounded_queue_t *q,
                const cleaner_platform_api_t *api)
{
  if (!q || !api)
    return;

  api->mutex_destroy(q->mutex);
  api->cond_destroy(q->not_empty);
  api->cond_destroy(q->not_full);

  free(q->buffer);
}

bool bq_push(bounded_queue_t *q,
             const cleaner_platform_api_t *api,
             void *item)
{
  if (!q || !api)
    return false;

  api->mutex_lock(q->mutex);

  while (q->size == q->capacity)
    api->cond_wait(q->not_full, q->mutex);

  q->buffer[q->tail] = item;
  q->tail = (q->tail + 1) % q->capacity;
  q->size++;

  api->cond_signal(q->not_empty);
  api->mutex_unlock(q->mutex);

  return true;
}

bool bq_pop(bounded_queue_t *q,
            const cleaner_platform_api_t *api,
            void **out)
{
  if (!q || !api || !out)
    return false;

  api->mutex_lock(q->mutex);

  while (q->size == 0)
    api->cond_wait(q->not_empty, q->mutex);

  *out = q->buffer[q->head];
  q->head = (q->head + 1) % q->capacity;
  q->size--;

  api->cond_signal(q->not_full);
  api->mutex_unlock(q->mutex);

  return true;
}