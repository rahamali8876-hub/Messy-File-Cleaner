// // // include/cleaner/system/tx_fs.h

#ifndef CLEANER_SYSTEM_TX_FS_H
#define CLEANER_SYSTEM_TX_FS_H

#include "cleaner/core/fs_interface.h"
#include "cleaner/wal/wal.h"

typedef struct {
  fs_interface_t *fs;
  cleaner_wal_t *wal;

} tx_fs_t;

/* initialization */

int tx_fs_init(tx_fs_t *tx, fs_interface_t *fs, cleaner_wal_t *wal);

/* operations */

int tx_fs_move(tx_fs_t *tx, const char *src, const char *dst);

int tx_fs_delete(tx_fs_t *tx, const char *path);

int tx_fs_mkdir(tx_fs_t *tx, const char *path);

/* recovery */

int tx_fs_recover(tx_fs_t *tx);

/* interface */

void tx_fs_build_interface(tx_fs_t *tx, fs_interface_t *out);

#endif