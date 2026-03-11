#ifndef CLEANER_CORE_PATH_UTILS_H
#define CLEANER_CORE_PATH_UTILS_H

#include "cleaner/core/fs_interface.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================
   PATH UTILITIES
   ============================================================ */

int path_create_recursive(fs_interface_t *fs, const char *path);

#ifdef __cplusplus
}
#endif

#endif