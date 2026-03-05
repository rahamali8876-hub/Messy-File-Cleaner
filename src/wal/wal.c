// src/wal/wal.c

#include "cleaner/wal/wal.h"
#include "cleaner/wal/wal_format.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern uint32_t cleaner_crc32(const void *data, size_t size);

struct cleaner_wal
{
    FILE *fp;
    uint64_t sequence;
};

FILE *cleaner_wal_file(cleaner_wal_t *wal)
{
    if (!wal)
        return NULL;
    return wal->fp;
}

static int wal_scan_sequence(FILE *fp, uint64_t *seq_out)
{
    cleaner_wal_record_header_t header;
    uint64_t last_seq = 0;

    rewind(fp);

    while (fread(&header, sizeof(header), 1, fp) == 1)
    {

        if (header.magic != CLEANER_WAL_MAGIC)
            break;

        if (header.version != CLEANER_WAL_VERSION)
            break;

        void *payload = malloc(header.payload_size);
        if (!payload)
            return -1;

        if (fread(payload, header.payload_size, 1, fp) != 1)
        {
            free(payload);
            break;
        }

        uint32_t crc = cleaner_crc32(payload, header.payload_size);
        free(payload);

        if (crc != header.crc32)
            break;

        last_seq = header.sequence;
    }

    *seq_out = last_seq;
    return 0;
}

int cleaner_wal_open(cleaner_wal_t **out, const char *path)
{
    cleaner_wal_t *wal = calloc(1, sizeof(*wal));
    if (!wal)
        return -1;

    wal->fp = fopen(path, "ab+");
    if (!wal->fp)
    {
        free(wal);
        return -1;
    }

    if (wal_scan_sequence(wal->fp, &wal->sequence) != 0)
    {
        fclose(wal->fp);
        free(wal);
        return -1;
    }

    fseek(wal->fp, 0, SEEK_END);

    *out = wal;
    return 0;
}

int cleaner_wal_append(cleaner_wal_t *wal,
                       const void *data,
                       size_t size,
                       uint64_t *sequence_out)
{
    cleaner_wal_record_header_t header;

    header.magic = CLEANER_WAL_MAGIC;
    header.version = CLEANER_WAL_VERSION;
    header.header_size = sizeof(header);
    header.sequence = ++wal->sequence;
    header.payload_size = (uint32_t)size;
    header.crc32 = cleaner_crc32(data, size);

    if (fwrite(&header, sizeof(header), 1, wal->fp) != 1)
        return -1;

    if (fwrite(data, size, 1, wal->fp) != 1)
        return -1;

    fflush(wal->fp);

#if defined(_WIN32)
    _commit(_fileno(wal->fp));
#else
    fsync(fileno(wal->fp));
#endif

    if (sequence_out)
        *sequence_out = header.sequence;

    return 0;
}

int cleaner_wal_recover(cleaner_wal_t *wal,
                        int (*apply_cb)(const void *, size_t))
{
    cleaner_wal_record_header_t header;

    rewind(wal->fp);

    while (fread(&header, sizeof(header), 1, wal->fp) == 1)
    {

        if (header.magic != CLEANER_WAL_MAGIC)
            break;

        void *payload = malloc(header.payload_size);
        if (!payload)
            return -1;

        if (fread(payload, header.payload_size, 1, wal->fp) != 1)
        {
            free(payload);
            break;
        }

        uint32_t crc = cleaner_crc32(payload, header.payload_size);
        if (crc != header.crc32)
        {
            free(payload);
            break;
        }

        if (apply_cb(payload, header.payload_size) != 0)
        {
            free(payload);
            return -1;
        }

        free(payload);
    }

    return 0;
}

int cleaner_wal_close(cleaner_wal_t *wal)
{
    if (!wal)
        return 0;

    fclose(wal->fp);
    free(wal);
    return 0;
}