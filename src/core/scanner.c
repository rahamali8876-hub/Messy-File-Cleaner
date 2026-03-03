
// // cleaner/src/scanner.c

#include "cleaner/core/scanner.h"

int cleaner_scan(const cleaner_platform_api_t *platform, const char *root_path,
                 cleaner_scan_callback callback, void *user_data) {
  if (!platform || !platform->fs_walk)
    return -1;

  return platform->fs_walk(root_path, callback, user_data);
}
