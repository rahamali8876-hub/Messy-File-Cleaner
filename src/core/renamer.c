
// cleaner/src/core/renamer.c

#include "cleaner/core/renamer.h"
#include <stdio.h>
#include <string.h>

static const char *get_extension(const char *name)
{
    const char *dot = strrchr(name, '.');
    if (!dot || dot == name)
        return "";
    return dot + 1;
}

int renamer_generate(
    const char *original_name,
    const renamer_timestamp_t *ts,
    uint64_t counter,
    char *output,
    size_t output_size)
{
    if (!original_name || !ts || !output || output_size == 0)
        return -1;

    const char *ext = get_extension(original_name);

    int written = snprintf(
        output,
        output_size,
        "%04d-%02d-%02d_%02d-%02d-%02d_%llu%s%s",
        ts->year,
        ts->month,
        ts->day,
        ts->hour,
        ts->minute,
        ts->second,
        (unsigned long long)counter,
        (*ext) ? "." : "",
        ext);

    if (written < 0 || (size_t)written >= output_size)
        return -1;

    return 0;
}