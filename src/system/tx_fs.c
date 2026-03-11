// // src/system/tx_fs.c

#include "cleaner/system/tx_fs.h"

/* ================= INIT ================= */

int tx_fs_init(tx_fs_t *tx, fs_interface_t *fs, cleaner_wal_t *wal)
{
  if (!tx || !fs || !wal)
    return -1;

  tx->fs = fs;
  tx->wal = wal;

  return 0;
}

/* ================= MOVE ================= */

int tx_fs_move(tx_fs_t *tx, const char *src, const char *dst)
{
  if (!tx || !src || !dst)
    return -1;

  cleaner_wal_log_move(tx->wal, src, dst);

  int r = tx->fs->move_file(tx->fs->context, src, dst);

  if (r != 0)
    return r;

  cleaner_wal_commit(tx->wal);

  return 0;
}

/* ================= DELETE ================= */

int tx_fs_delete(tx_fs_t *tx, const char *path)
{
  if (!tx || !path)
    return -1;

  cleaner_wal_log_delete(tx->wal, path);

  int r = tx->fs->remove_file(tx->fs->context, path);

  if (r != 0)
    return r;

  cleaner_wal_commit(tx->wal);

  return 0;
}

/* ================= MKDIR ================= */

int tx_fs_mkdir(tx_fs_t *tx, const char *path)
{
  if (!tx || !path)
    return -1;

  cleaner_wal_log_mkdir(tx->wal, path);

  int r = tx->fs->create_directory(tx->fs->context, path);

  if (r != 0)
    return r;

  cleaner_wal_commit(tx->wal);

  return 0;
}

/* ================= INTERFACE WRAPPERS ================= */

static int tx_fs_move_wrapper(void *context, const char *src, const char *dst)
{
  tx_fs_t *tx = (tx_fs_t *)context;
  return tx_fs_move(tx, src, dst);
}

static int tx_fs_delete_wrapper(void *context, const char *path)
{
  tx_fs_t *tx = (tx_fs_t *)context;
  return tx_fs_delete(tx, path);
}

static int tx_fs_mkdir_wrapper(void *context, const char *path)
{
  tx_fs_t *tx = (tx_fs_t *)context;
  return tx_fs_mkdir(tx, path);
}

/* ================= INTERFACE BUILDER ================= */

void tx_fs_build_interface(tx_fs_t *tx, fs_interface_t *out)
{
  if (!tx || !out)
    return;

  out->context = tx;

  out->move_file = tx_fs_move_wrapper;

  out->remove_file = tx_fs_delete_wrapper;

  out->create_directory = tx_fs_mkdir_wrapper;

  out->list_directory = tx->fs->list_directory;
}

/* ================= RECOVERY ================= */

int tx_fs_recover(tx_fs_t *tx)
{
  if (!tx)
    return -1;

  return cleaner_wal_replay(tx->wal, tx->fs);
}