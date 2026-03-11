// // include/cleaner/wal/wal.h

#ifndef CLEANER_WAL_H
#define CLEANER_WAL_H

#include "cleaner/core/fs_interface.h"
#include <stdio.h>

typedef struct {
  FILE *file;
} cleaner_wal_t;

/* internal access for checkpoint system */
FILE *cleaner_wal_file(cleaner_wal_t *wal);

/* open / close */

int cleaner_wal_open(cleaner_wal_t *wal, const char *path);

void cleaner_wal_close(cleaner_wal_t *wal);

void cleaner_wal_commit(cleaner_wal_t *wal);

/* operations */

void cleaner_wal_log_move(cleaner_wal_t *wal, const char *src, const char *dst);

void cleaner_wal_log_delete(cleaner_wal_t *wal, const char *path);

void cleaner_wal_log_mkdir(cleaner_wal_t *wal, const char *path);

/* recovery */

int cleaner_wal_replay(cleaner_wal_t *wal, fs_interface_t *fs);

/* internal access for checkpoint system */
FILE *cleaner_wal_file(cleaner_wal_t *wal);

#endif