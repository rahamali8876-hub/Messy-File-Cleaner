// src/system/time_adapter.c

#include "cleaner/system/time_adapter.h"

int time_adapter_init(time_adapter_t *ta,
                      const cleaner_platform_api_t *platform)
{
  if (!ta || !platform)
    return -1;

  if (platform->abi_version != CLEANER_PLATFORM_ABI_VERSION)
    return -1;

  if (!platform->time_now)
    return -1;

  ta->platform = platform;
  return 0;
}

int time_adapter_now(time_adapter_t *ta,
                     renamer_timestamp_t *out)
{
  if (!ta || !out || !ta->platform)
    return -1;

  cleaner_time_t pt;

  if (ta->platform->time_now(&pt) != 0)
    return -1;

  out->year = pt.year;
  out->month = pt.month;
  out->day = pt.day;
  out->hour = pt.hour;
  out->minute = pt.minute;
  out->second = pt.second;

  return 0;
}