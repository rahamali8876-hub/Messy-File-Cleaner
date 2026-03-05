// include/cleaner/wal/wal_tx.h

#ifndef CLEANER_WAL_TX_H
#define CLEANER_WAL_TX_H

#include <stdint.h>
#include "cleaner/wal/wal.h"

int cleaner_wal_begin(
    cleaner_wal_t *wal,
    uint64_t tx_id);

int cleaner_wal_log_move(
    cleaner_wal_t *wal,
    uint64_t tx_id,
    const char *src,
    const char *dst);

int cleaner_wal_commit(
    cleaner_wal_t *wal,
    uint64_t tx_id);

#endif