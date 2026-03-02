// include/cleaner/system/time_adapter.h

#ifndef CLEANER_SYSTEM_TIME_ADAPTER_H
#define CLEANER_SYSTEM_TIME_ADAPTER_H

#include "cleaner/core/renamer.h"
#include "cleaner/platform/platform_api.h"

typedef struct
{
    const cleaner_platform_api_t *platform;
} time_adapter_t;

int time_adapter_init(
    time_adapter_t *ta,
    const cleaner_platform_api_t *platform);

int time_adapter_now(
    time_adapter_t *ta,
    renamer_timestamp_t *out);

#endif