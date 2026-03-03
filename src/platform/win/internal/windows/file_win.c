// src/platform/win/internal/windows/file_win.c

#include "cleaner/platform/windows/file_win.h"

#include <stdio.h>
#include <stdlib.h>

struct cleaner_file {
  FILE *fp;
};

cleaner_file_t *win_file_open(const char *path, const char *mode) {
  if (!path || !mode)
    return NULL;

  FILE *fp = fopen(path, mode);
  if (!fp)
    return NULL;

  cleaner_file_t *file = malloc(sizeof(cleaner_file_t));

  if (!file) {
    fclose(fp);
    return NULL;
  }

  file->fp = fp;
  return file;
}

int win_file_write(cleaner_file_t *file, const void *data, size_t size) {
  if (!file || !file->fp || !data)
    return -1;

  return (int)fwrite(data, 1, size, file->fp);
}

int win_file_flush(cleaner_file_t *file) {
  if (!file || !file->fp)
    return -1;

  return fflush(file->fp);
}

void win_file_close(cleaner_file_t *file) {
  if (!file)
    return;

  if (file->fp)
    fclose(file->fp);

  free(file);
}