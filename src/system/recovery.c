// src/system/recovery.c

#include "cleaner/system/recovery.h"
#include <string.h>
#include <stdio.h>

#define REC_BUF 1024

static int recovery_apply(const void *data, size_t size)
{
    if (!data || size == 0 || size >= REC_BUF)
        return -1;

    char buffer[REC_BUF];
    memcpy(buffer, data, size);
    buffer[size] = '\0';

    /* Production engine:
       parse BEGIN/COMMIT pairs,
       re-run idempotent operations.
    */

    /* Currently safe no-op */
    return 0;
}

int recovery_init(recovery_engine_t *r,
                  cleaner_wal_t *wal)
{
    if (!r || !wal)
        return -1;

    r->wal = wal;
    return 0;
}

int recovery_run(recovery_engine_t *r)
{
    if (!r || !r->wal)
        return -1;

    return cleaner_wal_recover(r->wal, recovery_apply);
}