// include/cleaner/wal/wal_ops.h

#ifndef CLEANER_WAL_OPS_H
#define CLEANER_WAL_OPS_H

#include <stdint.h>

#define WAL_OP_BEGIN 1
#define WAL_OP_MOVE 2
#define WAL_OP_DELETE 3
#define WAL_OP_COMMIT 4

#pragma pack(push, 1)

typedef struct {
  uint32_t op;
  uint64_t tx_id;
} wal_op_begin_t;

typedef struct {
  uint32_t op;
  uint64_t tx_id;

  char src[260];
  char dst[260];

} wal_op_move_t;

typedef struct {
  uint32_t op;
  uint64_t tx_id;
} wal_op_commit_t;

#pragma pack(pop)

#endif