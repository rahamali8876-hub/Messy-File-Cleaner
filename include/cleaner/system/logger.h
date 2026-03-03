
// cleaner/system/logger.h
#ifndef CLEANER_SYSTEM_LOGGER_H
#define CLEANER_SYSTEM_LOGGER_H

#include "cleaner/platform/platform_api.h"

typedef struct {
  const cleaner_platform_api_t *platform;
  cleaner_mutex_t *mutex;
  cleaner_file_t *file;
} logger_t;

int logger_init(logger_t *log, const cleaner_platform_api_t *platform,
                const char *directory);

void logger_shutdown(logger_t *log);

void logger_log(logger_t *log, const char *level, const char *fmt, ...);

#endif