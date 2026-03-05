// src/system/memory_pool.c
#include "cleaner/system/memory_pool.h"
#include <stdlib.h>
#include <string.h>

int memory_pool_init(memory_pool_t *pool,
                     const cleaner_platform_api_t *api,
                     size_t block_size,
                     size_t capacity)
{
    if (!pool || !api || block_size == 0 || capacity == 0)
        return -1;

    pool->block_size = block_size;
    pool->capacity = capacity;
    pool->api = api;

    pool->buffer = malloc(block_size * capacity);
    if (!pool->buffer)
        return -1;

    pool->free_list = malloc(sizeof(void *) * capacity);
    if (!pool->free_list)
        return -1;

    for (size_t i = 0; i < capacity; i++)
        pool->free_list[i] =
            (char *)pool->buffer + (i * block_size);

    pool->free_count = capacity;

    pool->mutex = api->mutex_create();
    if (!pool->mutex)
        return -1;

    return 0;
}

void *memory_pool_alloc(memory_pool_t *pool)
{
    if (!pool)
        return NULL;

    pool->api->mutex_lock(pool->mutex);

    if (pool->free_count == 0)
    {
        pool->api->mutex_unlock(pool->mutex);
        return NULL;
    }

    void *ptr = pool->free_list[--pool->free_count];

    pool->api->mutex_unlock(pool->mutex);
    return ptr;
}

void memory_pool_free(memory_pool_t *pool, void *ptr)
{
    if (!pool || !ptr)
        return;

    pool->api->mutex_lock(pool->mutex);

    if (pool->free_count < pool->capacity)
        pool->free_list[pool->free_count++] = ptr;

    pool->api->mutex_unlock(pool->mutex);
}

void memory_pool_destroy(memory_pool_t *pool)
{
    if (!pool)
        return;

    pool->api->mutex_destroy(pool->mutex);
    free(pool->buffer);
    free(pool->free_list);
}

// task_ctx_t *ctx =
//     memory_pool_alloc(sched->task_pool);