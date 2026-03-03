#include <stdio.h>
#include <time.h>
#include "cleaner/core/core_run.h"
#include "cleaner/core/processor.h"
#include "cleaner/core/fs_interface.h"
#include "cleaner/core/config.h"
#include "cleaner/core/error.h"
#include "cleaner/core/fs_types.h"
#include "cleaner/core/organizer.h"
#include "cleaner/core/renamer.h"
#include "cleaner/core/scanner.h"
#include "cleaner/core/task_context.h"

#include "src/core/core_run.c"
#include "src/core/processor.c"
#include "src/core/error.c"
#include "src/core/organizer.c"
#include "src/core/renamer.c"
#include "src/core/scanner.c"
#include "src/core/task_context.c"

int main()
{
    clock_t start = clock();
    for (int i = 0; i < 100000; ++i)
    {
        process_task_dummy(); // Replace with actual core function
    }
    clock_t end = clock();
    double duration = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Core benchmark: %.6f seconds\n", duration);
    return 0;
}