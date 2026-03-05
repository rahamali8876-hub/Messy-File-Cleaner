// src/platform/win/internal/windows/file_win.c

#include "cleaner/platform/windows/file_win.h"
#include "cleaner/core/fs_interface.h"

#include <stdio.h>
#include <stdlib.h>

#include <windows.h>

static int fs_move(
    const char *src,
    const char *dst)
{
  if (MoveFileExA(
          src,
          dst,
          MOVEFILE_REPLACE_EXISTING |
              MOVEFILE_WRITE_THROUGH) == 0)
    return -1;

  return 0;
}

static int fs_remove(
    const char *path)
{
  if (DeleteFileA(path) == 0)
    return -1;

  return 0;
}

fs_interface_t cleaner_fs_win =
    {
        .move = fs_move,
        .remove = fs_remove};

struct cleaner_file_s
{
  FILE *fp;
};

cleaner_file_t *win_file_open(const char *path, const char *mode)
{
  if (!path || !mode)
    return NULL;

  FILE *fp = fopen(path, mode);
  if (!fp)
    return NULL;

  cleaner_file_t *file = malloc(sizeof(cleaner_file_t));

  if (!file)
  {
    fclose(fp);
    return NULL;
  }

  file->fp = fp;
  return file;
}

int win_file_write(cleaner_file_t *file, const void *data, size_t size)
{
  if (!file || !file->fp || !data)
    return -1;

  return (int)fwrite(data, 1, size, file->fp);
}

int win_file_flush(cleaner_file_t *file)
{
  if (!file || !file->fp)
    return -1;

  return fflush(file->fp);
}

void win_file_close(cleaner_file_t *file)
{
  if (!file)
    return;

  if (file->fp)
    fclose(file->fp);

  free(file);
}