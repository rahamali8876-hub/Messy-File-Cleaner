// include/cleaner/system/slab_pool.h

#ifndef CLEANER_SYSTEM_SLAB_POOL_H
#define CLEANER_SYSTEM_SLAB_POOL_H

#include "cleaner/platform/platform_api.h"
#include <stddef.h>

typedef struct slab slab_t;

typedef struct {
  slab_t *slabs;
  size_t object_size;
  size_t objects_per_slab;
  cleaner_mutex_t *mutex;
  const cleaner_platform_api_t *platform;
} slab_pool_t;

int slab_pool_init(slab_pool_t *pool, const cleaner_platform_api_t *platform,
                   size_t object_size, size_t objects_per_slab);

void *slab_alloc(slab_pool_t *pool);
void slab_free(slab_pool_t *pool, void *ptr);

void slab_pool_destroy(slab_pool_t *pool);

#endif