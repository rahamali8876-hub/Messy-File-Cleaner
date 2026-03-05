#ifndef CLEANER_SYSTEM_LF_EXECUTOR_H
#define CLEANER_SYSTEM_LF_EXECUTOR_H

#include <stdatomic.h>
#include <stddef.h>
#include "cleaner/platform/platform_api.h"
#include "cleaner/core/error.h"

typedef struct
{
    void (*fn)(void *);
    void *arg;
} lf_task_t;

typedef struct
{
    lf_task_t *buffer;
    size_t capacity;

    atomic_size_t head;
    atomic_size_t tail;

    atomic_int stop;

    cleaner_threadpool_t *workers;
    const cleaner_platform_api_t *platform;
} lf_executor_t;

error_t lf_executor_create(const cleaner_platform_api_t *platform,
                           int threads,
                           size_t capacity,
                           lf_executor_t **out);

error_t lf_executor_submit(lf_executor_t *exec,
                           void (*fn)(void *),
                           void *arg);

void lf_executor_destroy(lf_executor_t *exec);

#endif