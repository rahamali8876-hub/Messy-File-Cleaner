// include/cleaner/platform/filesystem_win.h

#ifndef CLEANER_PLATFORM_FILESYSTEM_H
#define CLEANER_PLATFORM_FILESYSTEM_H

#include <stdbool.h>
#include "cleaner/platform/platform_api.h"

/* Use ABI types directly */

int cleaner_fs_walk(
    const char *root_path,
    cleaner_fs_callback_t callback,
    void *user_data);

bool cleaner_fs_exists(const char *path);
bool cleaner_fs_is_directory(const char *path);
bool cleaner_fs_create_directory(const char *path);
bool cleaner_fs_rename(const char *old_path,
                       const char *new_path);

bool cleaner_fs_get_modification_time(
    const char *path_utf8,
    int *year,
    int *month,
    int *day);

bool cleaner_fs_get_creation_time(
    const char *path,
    int *year,
    int *month,
    int *day);

bool cleaner_fs_get_modified_time(
    const char *path,
    int *year,
    int *month,
    int *day);

#endif