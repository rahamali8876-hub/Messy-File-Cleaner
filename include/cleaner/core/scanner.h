// include/cleaner/core/scanner.h
// Parallel recursive directory scanner (CORE interface)

#ifndef CLEANER_CORE_SCANNER_H
#define CLEANER_CORE_SCANNER_H

#include "cleaner/core/fs_interface.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Recursive directory statistics
 */
typedef struct {
  uint64_t total_files;
  uint64_t total_dirs;
  uint64_t processed_files;
} scanner_stats_t;

/**
 * File processing callback
 * Called for each file found during scan
 */
typedef int (*scanner_callback_t)(const fs_entry_t *entry, void *context);

/**
 * Scan directory recursively (breadth-first)
 *
 * @param fs: filesystem interface
 * @param root_path: starting directory
 * @param callback: function called for each entry
 * @param context: passed to callback
 * @return: 0 on success, -1 on error
 */
int scanner_recursive(const fs_interface_t *fs, const char *root_path,
                      scanner_callback_t callback, void *context);

#ifdef __cplusplus
}
#endif

#endif // CLEANER_CORE_SCANNER_H
