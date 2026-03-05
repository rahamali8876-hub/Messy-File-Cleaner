// src/system/tx_engine.c

#include "cleaner/system/tx_engine.h"
#include <stdlib.h>
#include <string.h>

static uint64_t g_tx_id = 1;

int tx_engine_init(tx_engine_t *engine,
                   cleaner_wal_t *wal,
                   fs_interface_t *fs)
{
    if (!engine || !wal || !fs)
        return -1;

    engine->wal = wal;
    engine->fs = fs;

    return 0;
}

int tx_begin(tx_engine_t *engine,
             tx_handle_t *out)
{
    if (!engine || !out)
        return -1;

    out->id = g_tx_id++;

    if (cleaner_wal_begin(engine->wal, out->id) != 0)
        return -1;

    return 0;
}

int tx_move(tx_engine_t *engine,
            tx_handle_t *tx,
            const char *src,
            const char *dst)
{
    if (!engine || !tx || !src || !dst)
        return -1;

    /* 1️⃣ Log intent first (WAL) */
    if (cleaner_wal_log_move(engine->wal,
                             tx->id,
                             src,
                             dst) != 0)
        return -1;

    /* 2️⃣ Perform actual operation */
    if (engine->fs->move(src, dst) != 0)
        return -1;

    return 0;
}

int tx_commit(tx_engine_t *engine,
              tx_handle_t *tx)
{
    if (!engine || !tx)
        return -1;

    if (cleaner_wal_commit(engine->wal, tx->id) != 0)
        return -1;

    return 0;
}