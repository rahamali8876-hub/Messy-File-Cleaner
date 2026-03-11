// src/system/fs_adapter.c

#include "cleaner/system/fs_adapter.h"
#include <stdlib.h>
#include <string.h>

/* -----------------------------------------------------------
   Bridge context
   ----------------------------------------------------------- */

typedef struct {
  int (*core_callback)(const fs_entry_t *, void *);
  void *core_ctx;
} bridge_ctx_t;

/* -----------------------------------------------------------
   Safe deep-copy bridge
   ----------------------------------------------------------- */

static int bridge_callback(const cleaner_fs_entry_t *platform_entry,
                           void *user_data) {
  bridge_ctx_t *bridge = (bridge_ctx_t *)user_data;

  if (!bridge || !platform_entry || !bridge->core_callback)
    return -1;

  fs_entry_t core_entry;
  memset(&core_entry, 0, sizeof(core_entry));

  if (platform_entry->path) {
    size_t len = strlen(platform_entry->path);
    char *copy = (char *)malloc(len + 1);
    if (!copy)
      return -1;

    memcpy(copy, platform_entry->path, len + 1);
    core_entry.path = copy;
  }

  core_entry.is_directory = platform_entry->is_directory;
  core_entry.modification_time = platform_entry->modification_time;

  int result = bridge->core_callback(&core_entry, bridge->core_ctx);

  free((void *)core_entry.path);

  return result;
}

/* -----------------------------------------------------------
   Interface implementations
   ----------------------------------------------------------- */

static int list_impl(void *context, const char *path,
                     int (*callback)(const fs_entry_t *, void *), void *ctx) {
  fs_adapter_t *adapter = (fs_adapter_t *)context;

  if (!adapter || !adapter->platform || !adapter->platform->fs_walk)
    return -1;

  bridge_ctx_t bridge;
  bridge.core_callback = callback;
  bridge.core_ctx = ctx;

  return adapter->platform->fs_walk(path, bridge_callback, &bridge);
}

static int move_impl(void *context, const char *src, const char *dst) {
  fs_adapter_t *adapter = (fs_adapter_t *)context;

  if (!adapter || !adapter->platform || !adapter->platform->fs_rename)
    return -1;

  return adapter->platform->fs_rename(src, dst);
}

static int mkdir_impl(void *context, const char *path) {
  fs_adapter_t *adapter = (fs_adapter_t *)context;

  if (!adapter || !adapter->platform || !adapter->platform->fs_mkdir)
    return -1;

  return adapter->platform->fs_mkdir(path);
}

/* -----------------------------------------------------------
   Public API
   ----------------------------------------------------------- */

int fs_adapter_init(fs_adapter_t *adapter,
                    const cleaner_platform_api_t *platform) {
  if (!adapter || !platform)
    return -1;

  if (platform->abi_version != CLEANER_PLATFORM_ABI_VERSION)
    return -1;

  adapter->platform = platform;
  return 0;
}

void fs_adapter_build_interface(fs_adapter_t *adapter, fs_interface_t *out) {
  if (!adapter || !out)
    return;

  out->context = adapter;
  out->list_directory = list_impl;
  out->move_file = move_impl;
  out->create_directory = mkdir_impl;
}