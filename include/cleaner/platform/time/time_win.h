// include/cleaner/platform/time_win.h

#ifndef CLEANER_PLATFORM_TIME_H
#define CLEANER_PLATFORM_TIME_H

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct
    {
        int year;
        int month;
        int day;
        int hour;
        int minute;
        int second;
    } platform_time_t;

    int platform_time_now(platform_time_t *out);

#ifdef __cplusplus
}
#endif

#endif