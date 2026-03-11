// include/cleaner/system/recovery.h

// #ifndef CLEANER_SYSTEM_RECOVERY_H
// #define CLEANER_SYSTEM_RECOVERY_H

// #include "cleaner/wal/wal.h"

// typedef struct
// {
//     cleaner_wal_t *wal;
// } recovery_engine_t;

// int recovery_init(recovery_engine_t *r,
//                   cleaner_wal_t *wal);

// int recovery_run(recovery_engine_t *r);

// #endif

#ifndef CLEANER_SYSTEM_RECOVERY_H
#define CLEANER_SYSTEM_RECOVERY_H

#include "cleaner/core/fs_interface.h"
#include "cleaner/wal/wal.h"

typedef struct {
  cleaner_wal_t *wal;
  fs_interface_t *fs;

} recovery_engine_t;

int recovery_init(recovery_engine_t *r, cleaner_wal_t *wal, fs_interface_t *fs);

int recovery_run(recovery_engine_t *r);

#endif