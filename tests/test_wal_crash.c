#include "cleaner/wal/wal.h"
#include "cleaner/system/wal_checkpoint.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Simulate a crash-safe write by appending data */
static int wal_append_dummy(cleaner_wal_t *wal, const char *msg)
{
    return cleaner_wal_append(wal, msg, strlen(msg), NULL);
}

int main(void)
{
    cleaner_wal_t *wal = NULL;
    wal_checkpoint_t *checkpoint = NULL;
    const char *wal_path = "test_wal.log";
    const char *chk_path = "test_wal.chk";

    /* 1️⃣ Open WAL */
    if (cleaner_wal_open(&wal, wal_path) != 0)
    {
        fprintf(stderr, "Failed to open WAL\n");
        return -1;
    }

    /* 2️⃣ Append dummy transactions */
    wal_append_dummy(wal, "TX_BEGIN_1");
    wal_append_dummy(wal, "TX_MOVE_1");
    wal_append_dummy(wal, "TX_COMMIT_1");

    wal_append_dummy(wal, "TX_BEGIN_2");
    wal_append_dummy(wal, "TX_MOVE_2");
    wal_append_dummy(wal, "TX_COMMIT_2");

    /* 3️⃣ Initialize checkpoint engine */
    if (wal_checkpoint_init(&checkpoint, wal, chk_path) != 0)
    {
        fprintf(stderr, "Failed to init checkpoint\n");
        cleaner_wal_close(wal);
        return -1;
    }

    /* 4️⃣ Run checkpoint (flush WAL to snapshot) */
    if (wal_checkpoint_run(checkpoint) != 0)
    {
        fprintf(stderr, "Checkpoint failed\n");
        wal_checkpoint_destroy(checkpoint);
        cleaner_wal_close(wal);
        return -1;
    }

    printf("✅ Checkpoint successful: %s\n", chk_path);

    /* 5️⃣ Simulate crash by closing WAL abruptly */
    cleaner_wal_close(wal);
    wal_checkpoint_destroy(checkpoint);

    /* 6️⃣ Recover WAL after crash */
    if (cleaner_wal_open(&wal, wal_path) != 0)
    {
        fprintf(stderr, "Failed to reopen WAL\n");
        return -1;
    }

    printf("✅ WAL reopened successfully after simulated crash\n");

    int apply_cb(const void *data, size_t size)
    {
        char buf[1024];
        if (size >= sizeof(buf))
            return -1;
        memcpy(buf, data, size);
        buf[size] = '\0';
        printf("Recovered WAL entry: %s\n", buf);
        return 0;
    }

    if (cleaner_wal_recover(wal, apply_cb) != 0)
    {
        fprintf(stderr, "WAL recovery failed\n");
        cleaner_wal_close(wal);
        return -1;
    }

    printf("✅ WAL recovery successful\n");

    cleaner_wal_close(wal);
    return 0;
}