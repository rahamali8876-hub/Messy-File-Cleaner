// // src/platform/win/internal/windows/file_win.c

// src/platform/windows/file_win.c

#include "cleaner/platform/windows/file_win.h"
#include "cleaner/core/fs_interface.h"

#include <stdio.h>
#include <string.h>
#include <windows.h>

/* ================= FUNCTION PROTOTYPES ================= */

static int fs_move_file(void *context, const char *src, const char *dst);
static int fs_create_directory(void *context, const char *path);
static int fs_list_directory(void *context, const char *path,
                             int (*callback)(const fs_entry_t *, void *),
                             void *ctx);
static int fs_remove_file(void *context, const char *path);

/* ================= GLOBAL FS OBJECT ================= */

fs_interface_t cleaner_fs_win = {.context = NULL,
                                 .create_directory = fs_create_directory,
                                 .move_file = fs_move_file,
                                 .remove_file = fs_remove_file,
                                 .list_directory = fs_list_directory};

/* ================= MOVE FILE ================= */

static int fs_move_file(void *context, const char *src, const char *dst) {
  (void)context;
  printf("MOVE %s -> %s\n", src, dst);
  if (MoveFileExA(src, dst,
                  MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH) == 0)
    return -1;

  return 0;
}

/* ================= CREATE DIRECTORY ================= */

static int fs_create_directory(void *context, const char *path) {
  (void)context;

  if (CreateDirectoryA(path, NULL) == 0)
    return -1;

  return 0;
}

/* ================= REMOVE FILE ================= */

static int fs_remove_file(void *context, const char *path) {
  (void)context;

  if (DeleteFileA(path) == 0)
    return -1;

  return 0;
}

/* ================= DIRECTORY SCANNER ================= */

static int fs_list_directory(void *context, const char *path,
                             int (*callback)(const fs_entry_t *, void *),
                             void *ctx) {
  (void)context;
  printf("SCAN %s\n", path);
  if (!path || !callback)
    return -1;

  char search_path[MAX_PATH];

  snprintf(search_path, sizeof(search_path), "%s\\*", path);

  WIN32_FIND_DATAA find_data;

  HANDLE h = FindFirstFileA(search_path, &find_data);

  if (h == INVALID_HANDLE_VALUE)
    return -1;

  do {
    const char *name = find_data.cFileName;

    if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0)
      continue;

    fs_entry_t entry;

    entry.path = name;
    entry.is_directory =
        (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;

    if (callback(&entry, ctx) != 0) {
      FindClose(h);
      return -1;
    }

  } while (FindNextFileA(h, &find_data));

  FindClose(h);

  return 0;
}