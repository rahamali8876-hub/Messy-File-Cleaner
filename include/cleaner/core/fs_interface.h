// cleaner/include/core/fs_interface.c

#ifndef CLEANER_CORE_FS_INTERFACE_H
#define CLEANER_CORE_FS_INTERFACE_H

#ifdef __cplusplus
extern "C" {
#endif

// Struct is stable. Never extend. Use versioning if needed._entry_t;

typedef struct {
  const char *path;
  int is_directory;
} fs_entry_t;

typedef struct fs_interface {

  int (*list_directory)(const char *path,
                        int (*callback)(const fs_entry_t *entry, void *ctx),
                        void *ctx);

  int (*move_file)(const char *src, const char *dst);

  int (*create_directory)(const char *path);

} fs_interface_t;

#ifdef __cplusplus
}
#endif

#endif