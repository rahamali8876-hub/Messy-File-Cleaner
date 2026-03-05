// src/wal/wal_crc32.c
#include <stdint.h>
#include <stddef.h>

static uint32_t crc32_table[256];

static void crc32_init(void)
{
    uint32_t poly = 0xEDB88320;
    for (uint32_t i = 0; i < 256; i++) {
        uint32_t crc = i;
        for (uint32_t j = 0; j < 8; j++) {
            if (crc & 1)
                crc = (crc >> 1) ^ poly;
            else
                crc >>= 1;
        }
        crc32_table[i] = crc;
    }
}

uint32_t cleaner_crc32(const void *data, size_t size)
{
    static int initialized = 0;
    if (!initialized) {
        crc32_init();
        initialized = 1;
    }

    const uint8_t *p = (const uint8_t *)data;
    uint32_t crc = 0xFFFFFFFF;

    for (size_t i = 0; i < size; i++)
        crc = (crc >> 8) ^ crc32_table[(crc ^ p[i]) & 0xFF];

    return crc ^ 0xFFFFFFFF;
}