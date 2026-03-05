// src/wal/wal_tx.c

#include "cleaner/wal/wal_tx.h"
#include "cleaner/wal/wal_ops.h"

#include <string.h>

int cleaner_wal_begin(
    cleaner_wal_t *wal,
    uint64_t tx_id)
{
    wal_op_begin_t op;

    op.op = WAL_OP_BEGIN;
    op.tx_id = tx_id;

    return cleaner_wal_append(
        wal,
        &op,
        sizeof(op),
        NULL);
}

int cleaner_wal_log_move(
    cleaner_wal_t *wal,
    uint64_t tx_id,
    const char *src,
    const char *dst)
{
    wal_op_move_t op;

    memset(&op, 0, sizeof(op));

    op.op = WAL_OP_MOVE;
    op.tx_id = tx_id;

    strncpy(op.src, src, sizeof(op.src) - 1);
    strncpy(op.dst, dst, sizeof(op.dst) - 1);

    return cleaner_wal_append(
        wal,
        &op,
        sizeof(op),
        NULL);
}

int cleaner_wal_commit(
    cleaner_wal_t *wal,
    uint64_t tx_id)
{
    wal_op_commit_t op;

    op.op = WAL_OP_COMMIT;
    op.tx_id = tx_id;

    return cleaner_wal_append(
        wal,
        &op,
        sizeof(op),
        NULL);
}