
#include <stdio.h>
#include <time.h>
#include "cleaner/platform/platform_api.h"
#include "cleaner/platform/filesystem/filesystem_win.h"
#include "cleaner/platform/threads/threads_win.h"
#include "cleaner/platform/sync/mutex_win.h"
#include "cleaner/platform/sync/atomic_win.h"
#include "cleaner/platform/threadpool/threadpool_win.h"
#include "cleaner/platform/windows/file_win.h"
#include "cleaner/platform/time/time_win.h"

#include "src/platform/platform_win.c"
#include "src/platform/filesystem/filesystem_win.c"
#include "src/platform/threads/threads_win.c"
#include "src/platform/sync/mutex_win.c"
#include "src/platform/sync/atomic_win.c"
#include "src/platform/threadpool/threadpool_win.c"
#include "src/platform/windows/file_win.c"
#include "src/platform/time/time_win.c"

int main()
{
    clock_t start = clock();
    for (int i = 0; i < 50000; ++i)
    {
        platform_call_dummy(); // Replace with actual platform function
    }
    clock_t end = clock();
    double duration = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Platform benchmark: %.6f seconds\n", duration);
    return 0;
}