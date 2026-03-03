// include/cleaner/system/threadpool_adapter.h

#ifndef CLEANER_SYSTEM_THREADPOOL_ADAPTER_H
#define CLEANER_SYSTEM_THREADPOOL_ADAPTER_H

#include "cleaner/core/executor_interface.h"

int build_threadpool_executor(executor_interface_t *out, int thread_count);

void destroy_threadpool_executor(void);

#endif