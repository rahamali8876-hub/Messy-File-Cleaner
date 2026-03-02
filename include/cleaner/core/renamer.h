// cleaner/include\renamer.h

#ifndef CLEANER_CORE_RENAMER_H
#define CLEANER_CORE_RENAMER_H

#include <stddef.h>
#include <stdint.h>

typedef struct
{
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
} renamer_timestamp_t;

int renamer_generate(
    const char *original_name,
    const renamer_timestamp_t *ts,
    uint64_t counter,
    char *output,
    size_t output_size);

#endif

// #ifndef CLEANER_CORE_RENAMER_H
// #define CLEANER_CORE_RENAMER_H

// #include <stddef.h>
// #include <stdint.h>

// #ifdef __cplusplus
// extern "C"
// {
// #endif

//     typedef struct
//     {
//         int year;
//         int month;
//         int day;
//         int hour;
//         int minute;
//         int second;
//     } renamer_timestamp_t;

//     /*
//      * Generate new filename.
//      *
//      * Parameters:
//      *   original_name  - input filename
//      *   ts             - timestamp provided by outer layer
//      *   counter        - unique counter (provided by caller)
//      *   output         - output buffer
//      *   output_size    - size of output buffer
//      *
//      * Returns:
//      *   0 on success
//      *  -1 on error
//      */
//     int renamer_generate(
//         const char *original_name,
//         const renamer_timestamp_t *ts,
//         uint64_t counter,
//         char *output,
//         size_t output_size);

// #ifdef __cplusplus
// }
// #endif

// #endif