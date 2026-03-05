// src/system/tx_fs.c

#include "cleaner/system/tx_fs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TX_RECORD_MAX 1024

/* ----------------------------------------------------------
   WAL record format (text-based for simplicity)
   ---------------------------------------------------------- */

static int write_record(tx_fs_t *tx, const char *record)
{
    return cleaner_wal_append(tx->wal,
                              record,
                              strlen(record),
                              NULL);
}

/* ----------------------------------------------------------
   INIT
   ---------------------------------------------------------- */

int tx_fs_init(tx_fs_t *tx,
               fs_interface_t *fs,
               cleaner_wal_t *wal)
{
    if (!tx || !fs || !wal)
        return -1;

    tx->fs = fs;
    tx->wal = wal;
    return 0;
}

/* ----------------------------------------------------------
   MOVE (Transactional)
   ---------------------------------------------------------- */

int tx_fs_move(tx_fs_t *tx,
               const char *src,
               const char *dst)
{
    if (!tx || !src || !dst)
        return -1;

    char record[TX_RECORD_MAX];

    /* 1️⃣ INTENT */
    snprintf(record, sizeof(record),
             "MOVE|%s|%s|BEGIN\n", src, dst);

    if (write_record(tx, record) != 0)
        return -1;

    /* 2️⃣ EXECUTE */
    if (tx->fs->move_file(tx->fs->context, src, dst) != 0)
        return -1;

    /* 3️⃣ COMMIT */
    snprintf(record, sizeof(record),
             "MOVE|%s|%s|COMMIT\n", src, dst);

    return write_record(tx, record);
}

/* ----------------------------------------------------------
   DELETE (Safe via rename-to-trash)
   ---------------------------------------------------------- */

static void build_trash_path(const char *path,
                             char *out,
                             size_t size)
{
    snprintf(out, size, "%s.trash", path);
}

int tx_fs_delete(tx_fs_t *tx,
                 const char *path)
{
    if (!tx || !path)
        return -1;

    char trash[512];
    build_trash_path(path, trash, sizeof(trash));

    char record[TX_RECORD_MAX];

    snprintf(record, sizeof(record),
             "DELETE|%s|%s|BEGIN\n", path, trash);

    if (write_record(tx, record) != 0)
        return -1;

    /* Rename instead of delete */
    if (tx->fs->move_file(tx->fs->context,
                          path,
                          trash) != 0)
        return -1;

    snprintf(record, sizeof(record),
             "DELETE|%s|%s|COMMIT\n", path, trash);

    return write_record(tx, record);
}

/* ----------------------------------------------------------
   MKDIR
   ---------------------------------------------------------- */

int tx_fs_mkdir(tx_fs_t *tx,
                const char *path)
{
    if (!tx || !path)
        return -1;

    char record[TX_RECORD_MAX];

    snprintf(record, sizeof(record),
             "MKDIR|%s|BEGIN\n", path);

    if (write_record(tx, record) != 0)
        return -1;

    if (tx->fs->create_directory(tx->fs->context, path) != 0)
        return -1;

    snprintf(record, sizeof(record),
             "MKDIR|%s|COMMIT\n", path);

    return write_record(tx, record);
}

/* ----------------------------------------------------------
   RECOVERY
   ---------------------------------------------------------- */

static int recovery_apply(const void *data, size_t size)
{
    char buffer[TX_RECORD_MAX];
    if (size >= sizeof(buffer))
        return -1;

    memcpy(buffer, data, size);
    buffer[size] = '\0';

    /* Incomplete operations ignored for now */
    /* Production engine would track BEGIN without COMMIT */

    return 0;
}

int tx_fs_recover(tx_fs_t *tx)
{
    if (!tx)
        return -1;

    return cleaner_wal_recover(tx->wal,
                               recovery_apply);
}