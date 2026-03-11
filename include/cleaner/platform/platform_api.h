#ifndef CLEANER_PLATFORM_PLATFORM_API_H
#define CLEANER_PLATFORM_PLATFORM_API_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stddef.h>
#include <time.h>

   /* ============================================================
      ABI VERSION
      ============================================================ */

#define CLEANER_PLATFORM_ABI_VERSION 1

/* Static build */
#define CLEANER_PLATFORM_API

   /* ============================================================
      OPAQUE TYPES
      ============================================================ */

   typedef struct cleaner_file_s cleaner_file_t;
   typedef struct cleaner_mutex_s cleaner_mutex_t;
   typedef struct cleaner_cond_s cleaner_cond_t;
   typedef struct cleaner_threadpool_s cleaner_threadpool_t;

   /* Thread function */
   typedef void (*cleaner_thread_fn)(void *arg);

   /* ============================================================
      TIME STRUCT
      ============================================================ */

   typedef struct
   {
      int year;
      int month;
      int day;
      int hour;
      int minute;
      int second;

   } cleaner_time_t;

   /* ============================================================
      FILESYSTEM ENTRY
      ============================================================ */

   typedef struct
   {
      const char *path;
      int is_directory;
      time_t modification_time;

   } cleaner_fs_entry_t;

   /* ============================================================
      PLATFORM API
      ============================================================ */

   typedef struct cleaner_platform_api_s
   {
      unsigned int abi_version;

      /* file api */

      cleaner_file_t *(*file_open)(const char *path, const char *mode);
      int (*file_write)(cleaner_file_t *, const void *, size_t);
      int (*file_flush)(cleaner_file_t *);
      void (*file_close)(cleaner_file_t *);

      /* filesystem */

      int (*fs_mkdir)(const char *path);
      int (*fs_rename)(const char *src, const char *dst);

      int (*fs_walk)(const char *path,
                     int (*callback)(const cleaner_fs_entry_t *, void *),
                     void *user_data);

      /* time */

      int (*time_now)(cleaner_time_t *);

      /* mutex */

      cleaner_mutex_t *(*mutex_create)(void);
      void (*mutex_lock)(cleaner_mutex_t *);
      void (*mutex_unlock)(cleaner_mutex_t *);
      void (*mutex_destroy)(cleaner_mutex_t *);

      /* condition variable */

      cleaner_cond_t *(*cond_create)(void);
      void (*cond_wait)(cleaner_cond_t *, cleaner_mutex_t *);
      void (*cond_signal)(cleaner_cond_t *);
      void (*cond_broadcast)(cleaner_cond_t *);
      void (*cond_destroy)(cleaner_cond_t *);

      /* threadpool */

      cleaner_threadpool_t *(*threadpool_create)(int threads, int flags);
      void (*threadpool_submit)(cleaner_threadpool_t *, cleaner_thread_fn, void *);
      void (*threadpool_destroy)(cleaner_threadpool_t *);

   } cleaner_platform_api_t;

   /* ============================================================
      API LOADER
      ============================================================ */

   CLEANER_PLATFORM_API
   const cleaner_platform_api_t *cleaner_platform_get_api(void);

#ifdef __cplusplus
}
#endif

#endif
