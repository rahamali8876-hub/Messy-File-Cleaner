
// cleaner/src/system/logger.c

#include "cleaner/system/logger.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

/* ============================================================
   Initialization
   ============================================================ */

int logger_init(logger_t *log, const cleaner_platform_api_t *platform,
                const char *directory) {
  printf("DEBUG: after mutex\n");
  printf("DEBUG: after mkdir\n");
  printf("DEBUG: after file_open\n");

  if (!log || !platform || !directory)
    return -1;

  memset(log, 0, sizeof(*log));

  log->platform = platform;

  // if (!platform->mutex_create ||
  //     !platform->file_open ||
  //     !platform->file_write ||
  //     !platform->file_flush)
  //     return -1;

  if (!platform->mutex_create) {
    printf("FAIL: mutex_create missing\n");
    return -1;
  }

  if (!platform->file_open) {
    printf("FAIL: file_open missing\n");
    return -1;
  }

  if (!platform->file_write) {
    printf("FAIL: file_write missing\n");
    return -1;
  }

  if (!platform->file_flush) {
    printf("FAIL: file_flush missing\n");
    return -1;
  }

  log->mutex = platform->mutex_create();
  if (!log->mutex)
    return -1;

  if (platform->fs_mkdir(directory) != 0)
    return -1;
  // int mk = platform->fs_mkdir(directory);

  // if (mk != 0)
  // {
  //     /* If already exists, ignore error */
  //     if (!platform->fs_exists ||
  //         !platform->fs_exists(directory))
  //     {
  //         return -1;
  //     }
  // }

  char path[512];
  snprintf(path, sizeof(path), "%s/cleaner.log", directory);

  log->file = platform->file_open(path, "a");
  if (!log->file)
    return -1;

  return 0;
}

/* ============================================================
   Shutdown
   ============================================================ */

void logger_shutdown(logger_t *log) {
  if (!log || !log->platform)
    return;

  const cleaner_platform_api_t *p = log->platform;

  p->mutex_lock(log->mutex);

  if (log->file) {
    p->file_close(log->file);
    log->file = NULL;
  }

  p->mutex_unlock(log->mutex);
  p->mutex_destroy(log->mutex);

  log->mutex = NULL;
}

/* ============================================================
   Logging
   ============================================================ */

void logger_log(logger_t *log, const char *level, const char *fmt, ...) {
  if (!log || !log->file || !log->mutex)
    return;

  const cleaner_platform_api_t *p = log->platform;

  p->mutex_lock(log->mutex);

  cleaner_time_t now;
  if (p->time_now(&now) != 0) {
    p->mutex_unlock(log->mutex);
    return;
  }

  char buffer[1024];

  int offset =
      snprintf(buffer, sizeof(buffer), "%04d-%02d-%02d %02d:%02d:%02d [%s] ",
               now.year, now.month, now.day, now.hour, now.minute, now.second,
               level ? level : "INFO");

  va_list args;
  va_start(args, fmt);

  vsnprintf(buffer + offset, sizeof(buffer) - offset, fmt, args);

  va_end(args);

  strncat(buffer, "\n", sizeof(buffer) - strlen(buffer) - 1);

  p->file_write(log->file, buffer, strlen(buffer));

  p->file_flush(log->file);

  p->mutex_unlock(log->mutex);
}