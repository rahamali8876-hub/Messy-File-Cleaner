// src/system/recovery.c

#include "cleaner/system/recovery.h"

int recovery_init(recovery_engine_t *r, cleaner_wal_t *wal,
                  fs_interface_t *fs)
{
  if (!r || !wal || !fs)
    return -1;

  r->wal = wal;
  r->fs = fs;

  return 0;
}

int recovery_run(recovery_engine_t *r)
{
  if (!r || !r->wal || !r->fs)
    return -1;

  return cleaner_wal_replay(r->wal, r->fs);
}