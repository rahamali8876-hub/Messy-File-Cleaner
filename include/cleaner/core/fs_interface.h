// cleaner/include/core/fs_interface.h

#ifndef CLEANER_CORE_FS_INTERFACE_H
#define CLEANER_CORE_FS_INTERFACE_H

#ifdef __cplusplus
extern "C"
{
#endif

  typedef struct
  {
    const char *path;
    int is_directory;
  } fs_entry_t;

  typedef struct fs_interface
  {

    void *context; /* ← principal fix */

    int (*list_directory)(void *context,
                          const char *path,
                          int (*callback)(const fs_entry_t *, void *),
                          void *ctx);

    int (*move_file)(void *context,
                     const char *src,
                     const char *dst);

    int (*create_directory)(void *context,
                            const char *path);

    int (*move)(
        const char *src,
        const char *dst);

    int (*remove)(
        const char *path);

  } fs_interface_t;

#ifdef __cplusplus
}
#endif

#endif