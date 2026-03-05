#include "cleaner/system/lf_executor.h"
#include <stdlib.h>

static void worker_loop(void *arg)
{
    lf_executor_t *exec = arg;

    while (!atomic_load(&exec->stop))
    {

        size_t head = atomic_load(&exec->head);
        size_t tail = atomic_load(&exec->tail);

        if (head == tail)
            continue;

        if (atomic_compare_exchange_weak(&exec->head,
                                         &head,
                                         head + 1))
        {

            lf_task_t task =
                exec->buffer[head % exec->capacity];

            task.fn(task.arg);
        }
    }
}

error_t lf_executor_create(const cleaner_platform_api_t *platform,
                           int threads,
                           size_t capacity,
                           lf_executor_t **out)
{
    if (!platform || !out || threads <= 0 || capacity == 0)
        return error_make(ERR_INTERNAL, "Invalid params");

    lf_executor_t *exec = calloc(1, sizeof(*exec));
    if (!exec)
        return error_make(ERR_INTERNAL, "Alloc failed");

    exec->capacity = capacity;
    exec->buffer = calloc(capacity, sizeof(lf_task_t));
    if (!exec->buffer)
        return error_make(ERR_INTERNAL, "Buffer alloc failed");

    exec->platform = platform;
    atomic_store(&exec->head, 0);
    atomic_store(&exec->tail, 0);
    atomic_store(&exec->stop, 0);

    exec->workers =
        platform->threadpool_create(threads, 0);

    if (!exec->workers)
        return error_make(ERR_INTERNAL, "Threadpool failed");

    for (int i = 0; i < threads; i++)
        platform->threadpool_submit(exec->workers,
                                    worker_loop,
                                    exec);

    *out = exec;
    return error_ok();
}

error_t lf_executor_submit(lf_executor_t *exec,
                           void (*fn)(void *),
                           void *arg)
{
    if (!exec || !fn)
        return error_make(ERR_INTERNAL, "Invalid submit");

    size_t tail = atomic_load(&exec->tail);
    size_t head = atomic_load(&exec->head);

    if ((tail - head) >= exec->capacity)
        return error_make(ERR_QUEUE_FULL, "Queue full");

    exec->buffer[tail % exec->capacity] =
        (lf_task_t){fn, arg};

    atomic_store(&exec->tail, tail + 1);

    return error_ok();
}

void lf_executor_destroy(lf_executor_t *exec)
{
    if (!exec)
        return;

    atomic_store(&exec->stop, 1);
    free(exec->buffer);
    free(exec);
}