
// // cleaner/src/scanner.c

#include "cleaner/core/scanner.h"

int cleaner_scan(
    const cleaner_platform_api_t *platform,
    const char *root_path,
    cleaner_scan_callback callback,
    void *user_data)
{
    if (!platform || !platform->fs_walk)
        return -1;

    return platform->fs_walk(
        root_path,
        callback,
        user_data);
}

// #include "cleaner/core/renamer.h"

// #include <stdio.h>
// #include <string.h>
// #include <ctype.h>

// #include "cleaner/core/scanner.h"

// int cleaner_scan(
//     const cleaner_platform_api_t *platform,
//     const char *root_path,
//     cleaner_scan_callback callback,
//     void *user_data)
// {
//     if (!platform || !platform->fs_walk)
//         return -1;

//     return platform->fs_walk(
//         root_path,
//         callback,
//         user_data);
// }

// /* Extract file extension */
// static const char *get_extension(const char *name)
// {
//     const char *dot = strrchr(name, '.');
//     if (!dot || dot == name)
//         return "";
//     return dot + 1;
// }

// /* Case-insensitive compare (portable) */
// static int str_icmp(const char *a, const char *b)
// {
//     while (*a && *b)
//     {
//         char ca = (char)tolower((unsigned char)*a);
//         char cb = (char)tolower((unsigned char)*b);
//         if (ca != cb)
//             return ca - cb;
//         a++;
//         b++;
//     }
//     return *a - *b;
// }

// int renamer_generate(
//     const char *original_name,
//     const renamer_timestamp_t *ts,
//     uint64_t counter,
//     char *output,
//     size_t output_size)
// {
//     if (!original_name || !ts || !output || output_size == 0)
//         return -1;

//     const char *ext = get_extension(original_name);

//     int written = snprintf(
//         output,
//         output_size,
//         "%04d-%02d-%02d_%02d-%02d-%02d_%llu%s%s",
//         ts->year,
//         ts->month,
//         ts->day,
//         ts->hour,
//         ts->minute,
//         ts->second,
//         (unsigned long long)counter,
//         (*ext) ? "." : "",
//         ext);

//     if (written < 0 || (size_t)written >= output_size)
//         return -1;

//     return 0;
// }