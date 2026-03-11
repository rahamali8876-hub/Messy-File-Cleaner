// include/cleaner/plugin_loader.h

#ifndef CLEANER_PLUGIN_LOADER_H
#define CLEANER_PLUGIN_LOADER_H

#include "cleaner/core/fs_plugin.h"
#include <windows.h>

typedef struct {
  HMODULE handle;
  cleaner_fs_plugin_t *plugin;
} loaded_plugin_t;

int plugin_load(const char *path, loaded_plugin_t *out);

#endif