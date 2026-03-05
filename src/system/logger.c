// // src/system/logger.c

// #include "cleaner/system/logger.h"
// #include <stdarg.h>
// #include <stdio.h>
// #include <string.h>

// static int replay_callback(const void *data, size_t size)
// {
//   /* During recovery we don't need to re-log.
//      Could extend to rebuild state if needed. */
//   (void)data;
//   (void)size;
//   return 0;
// }

// int logger_init(logger_t *log,
//                 const cleaner_platform_api_t *platform,
//                 const char *directory)
// {
//   if (!log || !platform || !directory)
//     int offset = snprint return -1;

//   if (platform->abi_version != CLEANER_PLATFORM_ABI_VERSION)
//     return -1;

//   memset(log, 0, sizeof(*log));
//   log->platform = platform;

//   log->mutex = platform->mutex_create();
//   if (!log->mutex)
//     return -1;

//   platform->fs_mkdir(directory);

//   char log_path[512];
//   snprintf(log_path, sizeof(log_path), "%s/cleaner.log", directory);

//   log->file = platform->file_open(log_path, "a");
//   if (!log->file)
//     return -1;

//   char wal_path[512];
//   snprintf(wal_path, sizeof(wal_path), "%s/cleaner.wal", directory);

//   if (cleaner_wal_open(&log->wal, wal_path) != 0)
//     return -1;

//   /* Recover WAL on startup */
//   cleaner_wal_recover(log->wal, replay_callback);

//   return 0;
// }

// void logger_shutdown(logger_t *log)
// {
//   if (!log || !log->platform)
//     return;

//   const cleaner_platform_api_t *p = log->platform;

//   p->mutex_lock(log->mutex);

//   if (log->file)
//     p->file_close(log->file);

//   if (log->wal)
//     cleaner_wal_close(log->wal);

//   p->mutex_unlock(log->mutex);
//   p->mutex_destroy(log->mutex);
// }

// void logger_log(logger_t *log,
//                 const char *level,
//                 const char *fmt, ...)
// {
//   if (!log || !log->platform || !log->file || !log->mutex)
//     return;

//   const cleaner_platform_api_t *p = log->platform;

//   p->mutex_lock(log->mutex);

//   cleaner_time_t now;
//   if (p->time_now(&now) != 0)
//   {
//     p->mutex_unlock(log->mutex);
//     return;
//   }

//   char buffer[1024];

//   size_t offset = (size_t)snprintf(buffer, sizeof(buffer),
//                                    "%04d-%02d-%02d %02d:%02d:%02d [%s] ",
//                                    now.year, now.month, now.day,
//                                    now.hour, now.minute, now.second,
//                                    level ? level : "INFO");

//   if (offset >= sizeof(buffer))
//     offset = sizeof(buffer) - 1;

//   va_list args;
//   va_start(args, fmt);

//   vsnprintf(buffer + offset,
//             sizeof(buffer) - offset,
//             fmt,
//             args);

//   va_end(args);

//   strncat(buffer, "\n",
//           sizeof(buffer) - strlen(buffer) - 1);

//   /* 1️⃣ Write to WAL first (durable) */
//   cleaner_wal_append(log->wal, buffer, strlen(buffer), NULL);

//   /* 2️⃣ Then write to actual log */
//   p->file_write(log->file, buffer, strlen(buffer));
//   p->file_flush(log->file);

//   p->mutex_unlock(log->mutex);
// }

#include "cleaner/system/logger.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

/* ========================================================= */
/* WAL Replay Callback                                       */
/* ========================================================= */

static int replay_callback(const void *data, size_t size)
{
  /* Currently unused. WAL recovery could rebuild state here. */
  (void)data;
  (void)size;
  return 0;
}

/* ========================================================= */
/* Logger Initialization                                     */
/* ========================================================= */

int logger_init(
    logger_t *log,
    const cleaner_platform_api_t *platform,
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

  /* Log file path */
  char log_path[512];
  int r = snprintf(
      log_path,
      sizeof(log_path),
      "%s/cleaner.log",
      directory);

  if (r < 0 || (size_t)r >= sizeof(log_path))
    return -1;

  log->file = platform->file_open(log_path, "a");
  if (!log->file)
    return -1;

  /* WAL path */
  char wal_path[512];

  r = snprintf(
      wal_path,
      sizeof(wal_path),
      "%s/cleaner.wal",
      directory);

  if (r < 0 || (size_t)r >= sizeof(wal_path))
    return -1;

  if (cleaner_wal_open(&log->wal, wal_path) != 0)
    return -1;

  /* Recover WAL on startup */
  cleaner_wal_recover(log->wal, replay_callback);

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

  if (log->wal)
  {
    cleaner_wal_close(log->wal);
    log->wal = NULL;
  }

  p->mutex_unlock(log->mutex);

  p->mutex_destroy(log->mutex);
  log->mutex = NULL;
}

/* ========================================================= */
/* Logging                                                   */
/* ========================================================= */

void logger_log(
    logger_t *log,
    const char *level,
    const char *fmt,
    ...)
{
  if (!log || !log->platform || !log->file || !log->mutex)
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

  int offset = snprintf(
      buffer,
      sizeof(buffer),
      "%04d-%02d-%02d %02d:%02d:%02d [%s] ",
      now.year,
      now.month,
      now.day,
      now.hour,
      now.minute,
      now.second,
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

  int written = vsnprintf(
      buffer + used,
      sizeof(buffer) - used,
      fmt,
      args);

  va_end(args);

  if (written < 0)
  {
    p->mutex_unlock(log->mutex);
    return;
  }

  used += (size_t)written;

  if (used >= sizeof(buffer))
    used = sizeof(buffer) - 1;

  /* Append newline safely */
  if (used < sizeof(buffer) - 1)
  {
    buffer[used++] = '\n';
    buffer[used] = '\0';
  }

  /* ----------------------------------------------------- */
  /* 1️⃣ Write to WAL first (durability)                   */
  /* ----------------------------------------------------- */

  cleaner_wal_append(
      log->wal,
      buffer,
      used,
      NULL);

  /* ----------------------------------------------------- */
  /* 2️⃣ Write to actual log file                          */
  /* ----------------------------------------------------- */

  p->file_write(
      log->file,
      buffer,
      used);

  p->file_flush(log->file);

  p->mutex_unlock(log->mutex);
}