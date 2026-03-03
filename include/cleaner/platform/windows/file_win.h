// include/cleaner/platform/windows/file_win.h

#ifndef CLEANER_PLATFORM_WINDOWS_FILE_WIN_H
#define CLEANER_PLATFORM_WINDOWS_FILE_WIN_H

#include "cleaner/platform/platform_api.h"
#include <stddef.h>

/* Windows file backend API (internal to platform layer) */

cleaner_file_t *win_file_open(const char *path, const char *mode);

int win_file_write(cleaner_file_t *file, const void *data, size_t size);

int win_file_flush(cleaner_file_t *file);

void win_file_close(cleaner_file_t *file);

#endif