// src/core/scanner.c
// Pure recursive directory scanner (CORE layer - single-threaded business
// logic)

#include "cleaner/core/scanner.h"
#include <stdio.h>

/**
 * Recursive directory scanner
 *
 * Delegates to filesystem interface's list_directory which handles
 * the actual recursion and traversal. This is a thin wrapper that
 * ensures proper error handling and interface contracts.
 */
int scanner_recursive(const fs_interface_t *fs, const char *root_path,
                      scanner_callback_t callback, void *context) {
  if (!fs || !root_path || !callback)
    return -1;

  if (!fs->list_directory) {
    fprintf(stderr, "FS interface missing list_directory\n");
    return -1;
  }

  // Delegate to filesystem interface
  // fs->list_directory handles recursive traversal internally
  // and calls callback for each entry found
  int result = fs->list_directory(fs->context, root_path, callback, context);

  if (result != 0) {
    fprintf(stderr, "Warning: Directory scan failed for: %s\n", root_path);
    return result;
  }

  return 0;
}
