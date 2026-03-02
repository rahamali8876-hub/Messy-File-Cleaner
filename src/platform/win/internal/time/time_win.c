
// 📄 src/platform/time/time_win.c

/* INTERNAL - DO NOT INCLUDE OUTSIDE PLATFORM */

#include <windows.h>
#include <stdint.h>
#include "cleaner/platform/time/time_win.h"

int platform_time_now(platform_time_t *out)
{
    SYSTEMTIME st;
    GetLocalTime(&st);

    out->year = st.wYear;
    out->month = st.wMonth;
    out->day = st.wDay;
    out->hour = st.wHour;
    out->minute = st.wMinute;
    out->second = st.wSecond;

    return 0;
}

// uint64_t cleaner_time_now_ns(void)
// {
//     LARGE_INTEGER freq;
//     LARGE_INTEGER counter;

//     QueryPerformanceFrequency(&freq);
//     QueryPerformanceCounter(&counter);

//     // return (uint64_t)((counter.QuadPart * 1000000000ULL) / freq.QuadPart);
//     return ((uint64_t)counter.QuadPart * 1000000000ULL) /
//            (uint64_t)freq.QuadPart;
// }