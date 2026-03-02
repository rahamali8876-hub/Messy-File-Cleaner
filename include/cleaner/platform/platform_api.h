// include/cleaner/platform/platform_api.h

#ifndef CLEANER_PLATFORM_API_H
#define CLEANER_PLATFORM_API_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /* ============================= */
    /* ABI Version                   */
    /* ============================= */

#define CLEANER_PLATFORM_ABI_VERSION 1

    /* ============================= */
    /* Opaque Handles                */
    /* ============================= */

    typedef struct cleaner_mutex cleaner_mutex_t;
    typedef struct cleaner_thread cleaner_thread_t;
    typedef struct cleaner_threadpool cleaner_threadpool_t;
    typedef struct cleaner_file cleaner_file_t; /* <-- ADDED */

    /* ============================= */
    /* Time                          */
    /* ============================= */

    typedef struct
    {
        int year;
        int month;
        int day;
        int hour;
        int minute;
        int second;
    } cleaner_time_t;

    /* ============================= */
    /* Filesystem                    */
    /* ============================= */

    typedef struct
    {
        const char *path;
        int is_directory;
    } cleaner_fs_entry_t;

    typedef int (*cleaner_fs_callback_t)(
        const cleaner_fs_entry_t *entry,
        void *user_data);

    /* ============================= */
    /* Platform API Table            */
    /* ============================= */

    typedef struct
    {
        uint32_t abi_version;

        /* -------- Filesystem -------- */
        int (*fs_walk)(const char *, cleaner_fs_callback_t, void *);
        int (*fs_mkdir)(const char *);
        int (*fs_rename)(const char *, const char *);
        int (*fs_exists)(const char *);
        int (*fs_is_dir)(const char *);

        /* -------- File I/O -------- */
        cleaner_file_t *(*file_open)(
            const char *path,
            const char *mode);

        int (*file_write)(
            cleaner_file_t *file,
            const void *data,
            size_t size);

        int (*file_flush)(
            cleaner_file_t *file);

        void (*file_close)(
            cleaner_file_t *file);

        /* -------- Mutex -------- */
        cleaner_mutex_t *(*mutex_create)(void);
        void (*mutex_lock)(cleaner_mutex_t *);
        void (*mutex_unlock)(cleaner_mutex_t *);
        void (*mutex_destroy)(cleaner_mutex_t *);

        /* -------- Threadpool -------- */
        cleaner_threadpool_t *(*threadpool_create)(
            int threads,
            int queue_size);

        void (*threadpool_submit)(
            cleaner_threadpool_t *,
            void (*)(void *),
            void *);

        void (*threadpool_wait)(/* <-- ADD THIS */
                                cleaner_threadpool_t *);

        void (*threadpool_destroy)(
            cleaner_threadpool_t *);

        // cleaner_threadpool_t *(*threadpool_create)(
        //     int threads,
        //     int queue_size);

        // void (*threadpool_submit)(
        //     cleaner_threadpool_t *,
        //     void (*)(void *),
        //     void *);

        // void (*threadpool_destroy)(
        //     cleaner_threadpool_t *);

        /* -------- Time -------- */
        int (*time_now)(cleaner_time_t *);

    } cleaner_platform_api_t;

    /* ============================= */
    /* Entry Point                   */
    /* ============================= */

    int cleaner_platform_get_api(
        cleaner_platform_api_t *out);

#ifdef __cplusplus
}
#endif

#endif