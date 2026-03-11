// include/cleaner/system/wal_checkpoint.h

#ifndef CLEANER_SYSTEM_WAL_CHECKPOINT_H
#define CLEANER_SYSTEM_WAL_CHECKPOINT_H

#include "cleaner/wal/wal.h"

/* Principal-grade WAL checkpoint interface */
typedef struct wal_checkpoint wal_checkpoint_t;

/* Initialize checkpoint engine on a WAL file */
int wal_checkpoint_init(wal_checkpoint_t **out, cleaner_wal_t *wal,
                        const char *checkpoint_path);

/* Perform a checkpoint (flush WAL to permanent snapshot) */
int wal_checkpoint_run(wal_checkpoint_t *cp);

/* Destroy checkpoint engine */
int wal_checkpoint_destroy(wal_checkpoint_t *cp);

#endif /* CLEANER_SYSTEM_WAL_CHECKPOINT_H */