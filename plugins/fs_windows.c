// cleaner/plugins/fs_windows.c

#include "cleaner/core/fs_plugin.h"
#include <windows.h>
#include <stdio.h>

typedef struct
{
    int dummy;
} windows_fs_context;

static int windows_move(
    void *ctx,
    const char *src,
    const char *dst)
{
    return MoveFileExA(src, dst, MOVEFILE_REPLACE_EXISTING)
               ? 0
               : -1;
}

static int windows_mkdir(
    void *ctx,
    const char *path)
{
    return CreateDirectoryA(path, NULL) ? 0 : -1;
}

static int windows_remove(
    void *ctx,
    const char *path)
{
    return DeleteFileA(path) ? 0 : -1;
}

static int windows_init(void **context)
{
    *context = NULL;
    return 0;
}

static void windows_shutdown(void *ctx)
{
}

static cleaner_fs_plugin_t plugin =
    {
        .name = "windows_fs",

        .init = windows_init,
        .shutdown = windows_shutdown,

        .fs =
            {
                .move_file = windows_move,
                .create_directory = windows_mkdir,
                .remove_file = windows_remove}};

__declspec(dllexport)
cleaner_fs_plugin_t *
cleaner_fs_plugin_create(void)
{
    return &plugin;
}