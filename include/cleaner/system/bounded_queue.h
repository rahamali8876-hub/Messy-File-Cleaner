// include/cleaner/system/bounded_queue.h

#ifndef CLEANER_SYSTEM_BOUNDED_QUEUE_H
#define CLEANER_SYSTEM_BOUNDED_QUEUE_H

#include "cleaner/platform/platform_api.h"
#include <stdbool.h>
#include <stddef.h>

typedef struct {
  void **buffer;

  size_t capacity;
  size_t head;
  size_t tail;
  size_t size;

  cleaner_mutex_t *mutex;
  cleaner_cond_t *not_empty;
  cleaner_cond_t *not_full;

} bounded_queue_t;

/* Initialize queue */
int bq_init(bounded_queue_t *q, const cleaner_platform_api_t *api,
            size_t capacity);

/* Destroy queue */
void bq_destroy(bounded_queue_t *q, const cleaner_platform_api_t *api);

/* Blocking push */
bool bq_push(bounded_queue_t *q, const cleaner_platform_api_t *api, void *item);

/* Blocking pop */
bool bq_pop(bounded_queue_t *q, const cleaner_platform_api_t *api, void **out);

#endif