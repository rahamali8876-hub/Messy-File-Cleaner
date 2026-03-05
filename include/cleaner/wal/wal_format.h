// include/cleaner/wal/wal_format.h
#ifndef CLEANER_WAL_FORMAT_H
#define CLEANER_WAL_FORMAT_H

#include <stdint.h>

#define CLEANER_WAL_MAGIC 0x434C5741u /* 'CLWA' */
#define CLEANER_WAL_VERSION 1

#pragma pack(push, 1)

typedef struct
{
    uint32_t magic;
    uint16_t version;
    uint16_t header_size;
    uint64_t sequence;
    uint32_t payload_size;
    uint32_t crc32;
} cleaner_wal_record_header_t;

#pragma pack(pop)

#endif /* CLEANER_WAL_FORMAT_H */