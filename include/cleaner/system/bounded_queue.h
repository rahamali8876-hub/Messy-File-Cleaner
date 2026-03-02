// include/cleaner/system/bounded_queue.h

#ifndef CLEANER_SYSTEM_BOUNDED_QUEUE_H
#define CLEANER_SYSTEM_BOUNDED_QUEUE_H

#include <stdbool.h>
#include "cleaner/platform/platform_api.h"

typedef struct
{
    void **buffer;
    int capacity;
    int head;
    int tail;
    int size;

    cleaner_mutex_t *mutex;
    cleaner_cond_t *not_empty;
    cleaner_cond_t *not_full;

} bounded_queue_t;

/* Must be called before using queue */
int bq_init(
    bounded_queue_t *q,
    const cleaner_platform_api_t *api,
    int capacity);

void bq_destroy(
    bounded_queue_t *q,
    const cleaner_platform_api_t *api);

bool bq_push(
    bounded_queue_t *q,
    const cleaner_platform_api_t *api,
    void *item);

bool bq_pop(
    bounded_queue_t *q,
    const cleaner_platform_api_t *api,
    void **out);

#endif