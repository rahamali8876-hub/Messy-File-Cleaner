
// cleaner/include/scanner.h

#ifndef CLEANER_CORE_SCANNER_H
#define CLEANER_CORE_SCANNER_H

#include "cleaner/platform/platform_api.h"

typedef int (*cleaner_scan_callback)(const cleaner_fs_entry_t *entry,
                                     void *user_data);

int cleaner_scan(const cleaner_platform_api_t *platform, const char *root_path,
                 cleaner_scan_callback callback, void *user_data);

#endif