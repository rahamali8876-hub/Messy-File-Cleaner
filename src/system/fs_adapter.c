// src/system/fs_adapter.c
// src/system/fs_adapter.c

#include "cleaner/system/fs_adapter.h"
#include <stdlib.h>
#include <string.h>

/* -----------------------------------------------------------
   GLOBAL ADAPTER CONTEXT
   (Safe because you only build one interface per process)
   ----------------------------------------------------------- */

static fs_adapter_t *g_active_adapter = NULL;

/* -----------------------------------------------------------
   Bridge context for deep-copy callback
   ----------------------------------------------------------- */

typedef struct {
  int (*core_callback)(const fs_entry_t *, void *);
  void *core_ctx;
} bridge_ctx_t;

static int bridge_callback(const cleaner_fs_entry_t *platform_entry,
                           void *user_data) {
  bridge_ctx_t *bridge = (bridge_ctx_t *)user_data;

  if (!bridge || !platform_entry)
    return -1;

  /* Deep copy path for safety */
  fs_entry_t core_entry;
  core_entry.path = strdup(platform_entry->path);
  core_entry.is_directory = platform_entry->is_directory;

  int result = bridge->core_callback(&core_entry, bridge->core_ctx);

  free((void *)core_entry.path);
  return result;
}

/* -----------------------------------------------------------
   Interface wrappers (MATCH fs_interface_t EXACTLY)
   ----------------------------------------------------------- */

static int interface_list(const char *path,
                          int (*callback)(const fs_entry_t *, void *),
                          void *ctx) {
  if (!g_active_adapter || !g_active_adapter->platform)
    return -1;

  bridge_ctx_t bridge;
  bridge.core_callback = callback;
  bridge.core_ctx = ctx;

  return g_active_adapter->platform->fs_walk(path, bridge_callback, &bridge);
}

static int interface_move(const char *src, const char *dst) {
  if (!g_active_adapter || !g_active_adapter->platform)
    return -1;

  return g_active_adapter->platform->fs_rename(src, dst);
}

static int interface_mkdir(const char *path) {
  if (!g_active_adapter || !g_active_adapter->platform)
    return -1;

  return g_active_adapter->platform->fs_mkdir(path);
}

/* -----------------------------------------------------------
   Public API
   ----------------------------------------------------------- */

int fs_adapter_init(fs_adapter_t *adapter,
                    const cleaner_platform_api_t *platform) {
  if (!adapter || !platform)
    return -1;

  adapter->platform = platform;
  return 0;
}

void fs_adapter_build_interface(fs_adapter_t *adapter, fs_interface_t *out) {
  if (!adapter || !out)
    return;

  /* Store active adapter */
  g_active_adapter = adapter;

  /* Assign correctly-typed wrappers */
  out->list_directory = interface_list;
  out->move_file = interface_move;
  out->create_directory = interface_mkdir;
}