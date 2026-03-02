// src/platform/platform_win.c

#include "cleaner/platform/platform_api.h"

#include "cleaner/platform/filesystem/filesystem_win.h"
#include "cleaner/platform/sync/mutex_win.h"
#include "cleaner/platform/threadpool/threadpool_win.h"
#include "cleaner/platform/time/time_win.h"

#include "cleaner/platform/windows/file_win.h"

/* ===================================================== */
/* FILE I/O IMPLEMENTATION (if in separate file, just   */
/* forward declare here instead)                        */
/* ===================================================== */

cleaner_file_t *win_file_open(const char *, const char *);
int win_file_write(cleaner_file_t *, const void *, size_t);
int win_file_flush(cleaner_file_t *);
void win_file_close(cleaner_file_t *);

/* ===================================================== */
/* FILESYSTEM ADAPTER                                   */
/* ===================================================== */

static int abi_fs_walk(
    const char *root,
    cleaner_fs_callback_t cb,
    void *user)
{
    return cleaner_fs_walk(root, cb, user);
}

static int abi_fs_mkdir(const char *path)
{
    return cleaner_fs_create_directory(path) ? 0 : -1;
}

static int abi_fs_rename(const char *oldp, const char *newp)
{
    return cleaner_fs_rename(oldp, newp) ? 0 : -1;
}

static int abi_fs_exists(const char *path)
{
    return cleaner_fs_exists(path) ? 1 : 0;
}

static int abi_fs_is_dir(const char *path)
{
    return cleaner_fs_is_directory(path) ? 1 : 0;
}

/* ===================================================== */
/* MUTEX ADAPTER                                         */
/* ===================================================== */

static cleaner_mutex_t *abi_mutex_create(void)
{
    return (cleaner_mutex_t *)cleaner_mutex_create();
}

static void abi_mutex_lock(cleaner_mutex_t *m)
{
    cleaner_mutex_lock((cleaner_mutex *)m);
}

static void abi_mutex_unlock(cleaner_mutex_t *m)
{
    cleaner_mutex_unlock((cleaner_mutex *)m);
}

static void abi_mutex_destroy(cleaner_mutex_t *m)
{
    cleaner_mutex_destroy((cleaner_mutex *)m);
}

/* ===================================================== */
/* THREADPOOL ADAPTER                                   */
/* ===================================================== */

/* ===================================================== */
/* THREADPOOL ADAPTER                                   */
/* ===================================================== */

static cleaner_threadpool_t *
win_threadpool_create(int threads, int queue_size)
{
    threadpool_t *tp = NULL;

    if (threadpool_init(&tp, threads, queue_size) != 0)
        return NULL;

    return (cleaner_threadpool_t *)tp;
}

static void win_threadpool_submit(
    cleaner_threadpool_t *tp,
    void (*fn)(void *),
    void *arg)
{
    threadpool_submit((threadpool_t *)tp, fn, arg);
}

static void win_threadpool_wait(
    cleaner_threadpool_t *tp)
{
    threadpool_wait_all((threadpool_t *)tp);
}

static void win_threadpool_destroy(
    cleaner_threadpool_t *tp)
{
    threadpool_shutdown((threadpool_t *)tp);
}

/* ===================================================== */
/* TIME ADAPTER                                          */
/* ===================================================== */

static int abi_time_now(cleaner_time_t *out)
{
    platform_time_t t;

    if (platform_time_now(&t) != 0)
        return -1;

    out->year = t.year;
    out->month = t.month;
    out->day = t.day;
    out->hour = t.hour;
    out->minute = t.minute;
    out->second = t.second;

    return 0;
}

/* ===================================================== */
/* ABI ENTRYPOINT                                        */
/* ===================================================== */

int cleaner_platform_get_api(cleaner_platform_api_t *out)
{

    if (!out)
        return -1;

    out->abi_version = CLEANER_PLATFORM_ABI_VERSION;

    if (out->abi_version != CLEANER_PLATFORM_ABI_VERSION)
        return -2;

    /* Filesystem */
    out->fs_walk = abi_fs_walk;
    out->fs_mkdir = abi_fs_mkdir;
    out->fs_rename = abi_fs_rename;
    out->fs_exists = abi_fs_exists;
    out->fs_is_dir = abi_fs_is_dir;

    /* File I/O */
    out->file_open = win_file_open;
    out->file_write = win_file_write;
    out->file_flush = win_file_flush;
    out->file_close = win_file_close;

    /* Mutex */
    out->mutex_create = abi_mutex_create;
    out->mutex_lock = abi_mutex_lock;
    out->mutex_unlock = abi_mutex_unlock;
    out->mutex_destroy = abi_mutex_destroy;

    /* Threadpool */
    out->threadpool_create = win_threadpool_create;
    out->threadpool_submit = win_threadpool_submit;
    out->threadpool_wait = win_threadpool_wait; /* <-- ADD */
    out->threadpool_destroy = win_threadpool_destroy;

    /* Time */
    out->time_now = abi_time_now;

    return 0;
}