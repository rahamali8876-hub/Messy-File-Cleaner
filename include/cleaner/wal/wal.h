// include/cleaner/wal/wal.h
#ifndef CLEANER_WAL_H
#define CLEANER_WAL_H
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

typedef struct cleaner_wal cleaner_wal_t;

/* Principal-grade accessor: get FILE* of WAL for checkpointing */
FILE *cleaner_wal_file(cleaner_wal_t *wal);

int cleaner_wal_open(cleaner_wal_t **out, const char *path);
int cleaner_wal_append(cleaner_wal_t *wal,
                       const void *data,
                       size_t size,
                       uint64_t *sequence_out);

int cleaner_wal_recover(cleaner_wal_t *wal,
                        int (*apply_cb)(const void *data, size_t size));

int cleaner_wal_close(cleaner_wal_t *wal);

#endif