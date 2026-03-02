// cleaner/src/core/processor.c
#include "cleaner/core/processor.h"
#include <stdio.h>

int processor_process_file(
    processor_t *processor,
    const fs_entry_t *entry,
    uint64_t id)
{
    if (!processor || !entry)
        return -1;

    printf("[Processor] id=%llu path=%s\n",
           (unsigned long long)id,
           entry->path);

    // TODO: replace with real logic: scan, rename, move
    return 0;
}

int processor_init(
    processor_t *p,
    const fs_interface_t *fs,
    int (*get_time_fn)(renamer_timestamp_t *),
    int dry_run)
{
    if (!p || !fs)
        return -1;

    p->fs = *fs;
    p->get_time = get_time_fn;
    p->dry_run = dry_run;
    return 0;
}