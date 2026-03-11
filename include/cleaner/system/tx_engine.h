// include/cleaner/system/tx_engine.h

#ifndef CLEANER_SYSTEM_TX_ENGINE_H
#define CLEANER_SYSTEM_TX_ENGINE_H

#include <stdint.h>

#include "cleaner/core/fs_interface.h"
#include "cleaner/wal/wal.h"

typedef struct {
  uint64_t id;
} tx_handle_t;

typedef struct {
  cleaner_wal_t *wal;
  fs_interface_t *fs;
} tx_engine_t;

int tx_engine_init(tx_engine_t *engine, cleaner_wal_t *wal, fs_interface_t *fs);

int tx_begin(tx_engine_t *engine, tx_handle_t *out);

int tx_move(tx_engine_t *engine, tx_handle_t *tx, const char *src,
            const char *dst);

int tx_commit(tx_engine_t *engine, tx_handle_t *tx);

#endif