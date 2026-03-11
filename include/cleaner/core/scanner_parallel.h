// include/cleaner/core/scanner_parallel.h

#ifndef CLEANER_CORE_SCANNER_PARALLEL_H
#define CLEANER_CORE_SCANNER_PARALLEL_H

#include "cleaner/core/processor.h"

int scanner_parallel_run(processor_t *processor, const char *root, int threads);

#endif