#ifndef CLEANER_WAL_CRC32_H
#define CLEANER_WAL_CRC32_H

#include <stddef.h>
#include <stdint.h>

uint32_t cleaner_crc32(const void *data, size_t size);

#endif