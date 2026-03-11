
// cleaner/include/scanner.h

#ifndef CLEANER_SYSTEM_SCANNER_H
#define CLEANER_SYSTEM_SCANNER_H

#include "cleaner/core/error.h"
#include "cleaner/core/processor.h"
// #include "cleaner/system/lf_executor.h"
#include "cleaner/system/mpmc_executor.h"

#ifdef __cplusplus
extern "C" {
#endif

error_t scanner_run(processor_t *processor, const char *root,
                    mpmc_executor_t *executor, int scan_threads);

#ifdef __cplusplus
}
#endif

#endif