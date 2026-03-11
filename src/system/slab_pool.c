// src/system/slab_pool.c

#include "cleaner/system/slab_pool.h"

#include <stdint.h>
#include <stdlib.h>

struct slab
{
  void *memory;
  void **free_list;
  size_t free_count;
  struct slab *next;
};

static slab_t *create_slab(slab_pool_t *pool);

int slab_pool_init(slab_pool_t *pool, const cleaner_platform_api_t *platform,
                   size_t object_size, size_t objects_per_slab)
{
  if (!pool || !platform || object_size == 0 || objects_per_slab == 0)
    return -1;

  pool->object_size = object_size;
  pool->objects_per_slab = objects_per_slab;
  pool->platform = platform;
  pool->slabs = NULL;

  pool->mutex = platform->mutex_create();

  if (!pool->mutex)
    return -1;

  return 0;
}

static slab_t *create_slab(slab_pool_t *pool)
{
  slab_t *slab = (slab_t *)calloc(1, sizeof(*slab));
  if (!slab)
    return NULL;

  slab->memory = malloc(pool->object_size * pool->objects_per_slab);

  slab->free_list = malloc(sizeof(void *) * pool->objects_per_slab);

  if (!slab->memory || !slab->free_list)
  {
    free(slab->memory);
    free(slab->free_list);
    free(slab);
    return NULL;
  }

  for (size_t i = 0; i < pool->objects_per_slab; ++i)
  {
    slab->free_list[i] = (char *)slab->memory + (i * pool->object_size);
  }

  slab->free_count = pool->objects_per_slab;

  slab->next = pool->slabs;
  pool->slabs = slab;

  return slab;
}

void *slab_alloc(slab_pool_t *pool)
{
  if (!pool)
    return NULL;

  pool->platform->mutex_lock(pool->mutex);

  slab_t *slab = pool->slabs;

  while (slab && slab->free_count == 0)
    slab = slab->next;

  if (!slab)
    slab = create_slab(pool);

  if (!slab)
  {
    pool->platform->mutex_unlock(pool->mutex);
    return NULL;
  }

  void *ptr = slab->free_list[--slab->free_count];

  pool->platform->mutex_unlock(pool->mutex);

  return ptr;
}

void slab_free(slab_pool_t *pool, void *ptr)
{
  if (!pool || !ptr)
    return;

  pool->platform->mutex_lock(pool->mutex);

  slab_t *slab = pool->slabs;

  while (slab)
  {
    char *start = (char *)slab->memory;
    char *end = start + (pool->object_size * pool->objects_per_slab);

    char *p = (char *)ptr;

    if (p >= start && p < end)
    {
      // Check for double-free by verifying pointer is not already in free list
      for (size_t i = 0; i < slab->free_count; i++)
      {
        if (slab->free_list[i] == ptr)
        {
          // Double-free detected, silently return
          pool->platform->mutex_unlock(pool->mutex);
          return;
        }
      }

      // Add to free list if there's space
      if (slab->free_count < pool->objects_per_slab)
      {
        slab->free_list[slab->free_count++] = ptr;
      }
      pool->platform->mutex_unlock(pool->mutex);
      return;
    }

    slab = slab->next;
  }

  pool->platform->mutex_unlock(pool->mutex);
}

void slab_pool_destroy(slab_pool_t *pool)
{
  if (!pool)
    return;

  slab_t *slab = pool->slabs;

  while (slab)
  {
    slab_t *next = slab->next;

    free(slab->memory);
    free(slab->free_list);
    free(slab);

    slab = next;
  }

  if (pool->platform && pool->mutex)
    pool->platform->mutex_destroy(pool->mutex);

  pool->slabs = NULL;
}