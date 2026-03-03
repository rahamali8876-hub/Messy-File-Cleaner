#include <stdio.h>
#include <time.h>

#include "cleaner/system/executor.h"
#include "cleaner/system/bounded_queue.h"
#include "cleaner/system/file_scheduler.h"
#include "cleaner/system/fs_adapter.h"
#include "cleaner/system/logger.h"
#include "cleaner/system/memory_pool.h"
#include "cleaner/system/recovery.h"
#include "cleaner/system/time_adapter.h"
#include "cleaner/system/wal.h"

#include "src/system/executor.c"
#include "src/system/bounded_queue.c"
#include "src/system/file_scheduler.c"
#include "src/system/fs_adapter.c"
#include "src/system/logger.c"
#include "src/system/memory_pool.c"
#include "src/system/recovery.c"
#include "src/system/time_adapter.c"
#include "src/system/wal.c"

int main()
{
    clock_t start = clock();
    for (int i = 0; i < 50000; ++i)
    {
        executor_run_dummy(); // Replace with actual system function
    }
    clock_t end = clock();
    double duration = (double)(end - start) / CLOCKS_PER_SEC;
    printf("System benchmark: %.6f seconds\n", duration);
    return 0;
}