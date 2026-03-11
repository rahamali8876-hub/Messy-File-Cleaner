#ifndef CLEANER_FS_PLUGIN_H
#define CLEANER_FS_PLUGIN_H

#include "cleaner/core/fs_interface.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  const char *name;

  int (*init)(void **context);
  void (*shutdown)(void *context);

  fs_interface_t fs;

} cleaner_fs_plugin_t;

/* exported symbol every plugin must provide */

typedef cleaner_fs_plugin_t *(*cleaner_fs_plugin_create_fn)(void);

#ifdef __cplusplus
}
#endif

#endif