// cleaner/include/core/fs_interface.h

#ifndef CLEANER_CORE_FS_INTERFACE_H
#define CLEANER_CORE_FS_INTERFACE_H

#ifdef __cplusplus
extern "C"
{
#endif

  /* -------------------------------------------------- */
  /* Filesystem entry                                   */
  /* -------------------------------------------------- */

#include <time.h>

  typedef struct
  {
    const char *path;
    int is_directory;
    time_t modification_time; /* File modification time */
  } fs_entry_t;

  /* -------------------------------------------------- */
  /* Filesystem interface                               */
  /* -------------------------------------------------- */

  typedef struct fs_interface
  {
    /* platform implementation context */
    void *context;

    /* directory scan */

    int (*list_directory)(void *context, const char *path,
                          int (*callback)(const fs_entry_t *, void *),
                          void *callback_ctx);

    /* file operations */

    int (*move_file)(void *context, const char *src, const char *dst);

    int (*remove_file)(void *context, const char *path);

    int (*create_directory)(void *context, const char *path);

  } fs_interface_t;

#ifdef __cplusplus
}
#endif

#endif
