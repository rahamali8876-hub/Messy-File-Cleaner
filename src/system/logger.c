// // src/system/logger.c

#include "cleaner/system/logger.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

/* ========================================================= */
/* Logger Initialization                                     */
/* ========================================================= */

int logger_init(logger_t *log, const cleaner_platform_api_t *platform,
                const char *directory)
{
  if (!log || !platform || !directory)
    return -1;

  if (platform->abi_version != CLEANER_PLATFORM_ABI_VERSION)
    return -1;

  memset(log, 0, sizeof(*log));

  log->platform = platform;

  /* Create mutex */

  log->mutex = platform->mutex_create();

  if (!log->mutex)
    return -1;

  /* Ensure directory exists */

  platform->fs_mkdir(directory);

  /* ---------------- Log file path ---------------- */

  char log_path[512];

  int r = snprintf(log_path, sizeof(log_path), "%s/cleaner.log", directory);

  if (r < 0 || (size_t)r >= sizeof(log_path))
    return -1;

  log->file = platform->file_open(log_path, "a");

  if (!log->file)
    return -1;

  /* ---------------- WAL path ---------------- */

  char wal_path[512];

  r = snprintf(wal_path, sizeof(wal_path), "%s/cleaner.wal", directory);

  if (r < 0 || (size_t)r >= sizeof(wal_path))
    return -1;

  if (cleaner_wal_open(&log->wal, wal_path) != 0)
    return -1;

  return 0;
}

/* ========================================================= */
/* Shutdown                                                  */
/* ========================================================= */

void logger_shutdown(logger_t *log)
{
  if (!log || !log->platform)
    return;

  const cleaner_platform_api_t *p = log->platform;

  p->mutex_lock(log->mutex);

  if (log->file)
  {
    p->file_close(log->file);
    log->file = NULL;
  }

  cleaner_wal_close(&log->wal);

  p->mutex_unlock(log->mutex);

  p->mutex_destroy(log->mutex);

  log->mutex = NULL;
}

/* ========================================================= */
/* Logging                                                   */
/* ========================================================= */

void logger_log(logger_t *log, const char *level, const char *fmt, ...)
{
  if (!log || !log->platform || !log->file)
    return;

  const cleaner_platform_api_t *p = log->platform;

  p->mutex_lock(log->mutex);

  cleaner_time_t now;

  if (p->time_now(&now) != 0)
  {
    p->mutex_unlock(log->mutex);
    return;
  }

  char buffer[1024];

  int offset =
      snprintf(buffer, sizeof(buffer), "%04d-%02d-%02d %02d:%02d:%02d [%s] ",
               now.year, now.month, now.day, now.hour, now.minute, now.second,
               level ? level : "INFO");

  if (offset < 0)
  {
    p->mutex_unlock(log->mutex);
    return;
  }

  size_t used = (size_t)offset;

  if (used >= sizeof(buffer))
    used = sizeof(buffer) - 1;

  /* Format message */

  va_list args;

  va_start(args, fmt);

  int written = vsnprintf(buffer + used, sizeof(buffer) - used, fmt, args);

  va_end(args);

  if (written < 0)
  {
    p->mutex_unlock(log->mutex);
    return;
  }

  used += (size_t)written;

  if (used >= sizeof(buffer))
    used = sizeof(buffer) - 1;

  /* Append newline */

  if (used < sizeof(buffer) - 1)
  {
    buffer[used++] = '\n';
    buffer[used] = '\0';
  }

  /* Write to log file */

  p->file_write(log->file, buffer, used);

  p->file_flush(log->file);

  p->mutex_unlock(log->mutex);
}
