// src/system/tx_engine.c

#include "cleaner/system/tx_engine.h"

#include <stdio.h>

int tx_engine_init(tx_engine_t *engine, cleaner_wal_t *wal,
                   fs_interface_t *fs)
{
  if (!engine || !wal || !fs)
    return -1;

  engine->wal = wal;
  engine->fs = fs;

  return 0;
}

int tx_begin(tx_engine_t *engine, tx_handle_t *out)
{
  if (!engine || !out)
    return -1;

  out->id = 0; /* unused in simple WAL */

  return 0;
}

int tx_move(tx_engine_t *engine, tx_handle_t *tx, const char *src,
            const char *dst)
{
  (void)tx; /* silence unused parameter warning */

  if (!engine || !src || !dst)
    return -1;

  /* 1️⃣ Write-Ahead Log */
  cleaner_wal_log_move(engine->wal, src, dst);

  /* 2️⃣ Execute filesystem operation */

  if (engine->fs->move_file(engine->fs->context, src, dst) != 0)
  {
    return -1;
  }

  return 0;
}

int tx_commit(tx_engine_t *engine, tx_handle_t *tx)
{
  (void)tx; /* silence unused parameter warning */

  if (!engine)
    return -1;

  /* flush WAL to disk */
  cleaner_wal_commit(engine->wal);

  return 0;
}