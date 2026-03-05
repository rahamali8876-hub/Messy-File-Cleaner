// include/cleaner/system/memory_pool.h

#ifndef CLEANER_SYSTEM_MEMORY_POOL_H
#define CLEANER_SYSTEM_MEMORY_POOL_H

#include <stddef.h>
#include "cleaner/platform/platform_api.h"

typedef struct
{
    size_t block_size;
    size_t capacity;

    void *buffer;
    void **free_list;
    size_t free_count;

    void *mutex;

    const cleaner_platform_api_t *api;

} memory_pool_t;

int memory_pool_init(memory_pool_t *pool,
                     const cleaner_platform_api_t *api,
                     size_t block_size,
                     size_t capacity);

void *memory_pool_alloc(memory_pool_t *pool);

void memory_pool_free(memory_pool_t *pool,
                      void *ptr);

void memory_pool_destroy(memory_pool_t *pool);

#endif