
// src/platform/platform_api.c

#define CLEANER_PLATFORM_BUILD
#include "cleaner/platform/platform_api.h"

#ifdef _WIN32

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>

/* ========================================================= */
/* OPAQUE STRUCTS                                            */
/* ========================================================= */

struct cleaner_file_s
{
  FILE *handle;
};

struct cleaner_mutex_s
{
  CRITICAL_SECTION cs;
};

struct cleaner_cond_s
{
  CONDITION_VARIABLE cv;
};

struct cleaner_threadpool_s
{
  PTP_POOL pool;
  TP_CALLBACK_ENVIRON env;
};

/* ========================================================= */
/* FILE                                                       */
/* ========================================================= */

static cleaner_file_t *win_file_open(const char *path, const char *mode)
{
  FILE *f = fopen(path, mode);
  if (!f)
    return NULL;

  cleaner_file_t *file = malloc(sizeof(*file));
  if (!file)
  {
    fclose(f);
    return NULL;
  }

  file->handle = f;
  return file;
}

static int win_file_write(cleaner_file_t *file, const void *data, size_t size)
{
  if (!file || !file->handle)
    return -1;
  return fwrite(data, 1, size, file->handle) == size ? 0 : -1;
}

static int win_file_flush(cleaner_file_t *file)
{
  if (!file || !file->handle)
    return -1;
  return fflush(file->handle);
}

static void win_file_close(cleaner_file_t *file)
{
  if (!file)
    return;
  if (file->handle)
    fclose(file->handle);
  free(file);
}

/* ========================================================= */
/* DIRECTORY                                                  */
/* ========================================================= */

static int win_fs_mkdir(const char *path)
{
  return CreateDirectoryA(path, NULL) ? 0 : -1;
}

static int win_fs_rename(const char *src, const char *dst)
{
  return MoveFileExA(src, dst, MOVEFILE_REPLACE_EXISTING) ? 0 : -1;
}

static int win_fs_walk(const char *path,
                       int (*callback)(const cleaner_fs_entry_t *, void *),
                       void *user_data)
{
  char search[MAX_PATH];
  snprintf(search, sizeof(search), "%s\\*", path);

  WIN32_FIND_DATAA data;
  HANDLE h = FindFirstFileA(search, &data);
  if (h == INVALID_HANDLE_VALUE)
    return -1;

  int result = 0;
  do
  {
    if (!strcmp(data.cFileName, ".") || !strcmp(data.cFileName, ".."))
      continue;

    // Build full path for entry
    size_t path_len = strlen(path) + 1 + strlen(data.cFileName) + 1;
    char *full_path = (char *)malloc(path_len);
    if (!full_path)
      return -1;

    snprintf(full_path, path_len, "%s\\%s", path, data.cFileName);

    cleaner_fs_entry_t entry;
    entry.path = full_path;
    entry.is_directory =
        (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;

    // Convert Windows FILETIME to Unix time_t
    FILETIME ft = data.ftLastWriteTime;
    ULARGE_INTEGER uli;
    uli.LowPart = ft.dwLowDateTime;
    uli.HighPart = ft.dwHighDateTime;

    // Windows FILETIME is in 100-nanosecond intervals since 1601-01-01
    // Unix time_t is seconds since 1970-01-01
    // Difference is 116444736000000000 (100-nanosecond intervals)
    static const unsigned long long FILETIME_UNIX_DIFF = 116444736000000000ULL;

    if (uli.QuadPart >= FILETIME_UNIX_DIFF)
    {
      entry.modification_time =
          (time_t)((uli.QuadPart - FILETIME_UNIX_DIFF) / 10000000);
    }
    else
    {
      entry.modification_time = 0;
    }

    if (callback(&entry, user_data) != 0)
    {
      free(full_path);
      result = -1;
      break;
    }

    free(full_path);

  } while (FindNextFileA(h, &data));

  FindClose(h);
  return result;
}

/* ========================================================= */
/* TIME                                                       */
/* ========================================================= */

static int win_time_now(cleaner_time_t *out)
{
  if (!out)
    return -1;

  SYSTEMTIME st;
  GetLocalTime(&st);

  out->year = st.wYear;
  out->month = st.wMonth;
  out->day = st.wDay;
  out->hour = st.wHour;
  out->minute = st.wMinute;
  out->second = st.wSecond;

  return 0;
}

/* ========================================================= */
/* MUTEX                                                      */
/* ========================================================= */

static cleaner_mutex_t *win_mutex_create(void)
{
  cleaner_mutex_t *m = malloc(sizeof(*m));
  if (!m)
    return NULL;

  InitializeCriticalSection(&m->cs);
  return m;
}

static void win_mutex_lock(cleaner_mutex_t *m)
{
  if (m)
    EnterCriticalSection(&m->cs);
}

static void win_mutex_unlock(cleaner_mutex_t *m)
{
  if (m)
    LeaveCriticalSection(&m->cs);
}

static void win_mutex_destroy(cleaner_mutex_t *m)
{
  if (!m)
    return;
  DeleteCriticalSection(&m->cs);
  free(m);
}

/* ========================================================= */
/* CONDITION VARIABLE                                        */
/* ========================================================= */

static cleaner_cond_t *win_cond_create(void)
{
  cleaner_cond_t *c = malloc(sizeof(*c));
  if (!c)
    return NULL;
  InitializeConditionVariable(&c->cv);
  return c;
}

static void win_cond_wait(cleaner_cond_t *c, cleaner_mutex_t *m)
{
  SleepConditionVariableCS(&c->cv, &m->cs, INFINITE);
}

static void win_cond_signal(cleaner_cond_t *c)
{
  WakeConditionVariable(&c->cv);
}

static void win_cond_broadcast(cleaner_cond_t *c)
{
  WakeAllConditionVariable(&c->cv);
}

static void win_cond_destroy(cleaner_cond_t *c) { free(c); }

/* ========================================================= */
/* THREADPOOL                                                */
/* ========================================================= */

/* Proper context struct (NO illegal casts) */
typedef struct
{
  cleaner_thread_fn fn;
  void *arg;
} win_tp_ctx_t;

static VOID CALLBACK win_tp_callback(PTP_CALLBACK_INSTANCE instance,
                                     PVOID context, PTP_WORK work)
{
  (void)instance;

  win_tp_ctx_t *ctx = (win_tp_ctx_t *)context;

  ctx->fn(ctx->arg);

  free(ctx);
  CloseThreadpoolWork(work);
}

static cleaner_threadpool_t *win_threadpool_create(int threads, int flags)
{
  (void)flags;

  if (threads <= 0)
    return NULL;

  cleaner_threadpool_t *tp = malloc(sizeof(*tp));
  if (!tp)
    return NULL;

  tp->pool = CreateThreadpool(NULL);
  if (!tp->pool)
  {
    free(tp);
    return NULL;
  }

  SetThreadpoolThreadMaximum(tp->pool, (DWORD)threads);
  SetThreadpoolThreadMinimum(tp->pool, 1);

  InitializeThreadpoolEnvironment(&tp->env);
  SetThreadpoolCallbackPool(&tp->env, tp->pool);

  return tp;
}

static void win_threadpool_submit(cleaner_threadpool_t *tp,
                                  cleaner_thread_fn fn, void *arg)
{
  if (!tp || !fn)
    return;

  win_tp_ctx_t *ctx = malloc(sizeof(*ctx));
  if (!ctx)
    return;

  ctx->fn = fn;
  ctx->arg = arg;

  PTP_WORK work = CreateThreadpoolWork(win_tp_callback, ctx, &tp->env);

  if (!work)
  {
    free(ctx);
    return;
  }

  SubmitThreadpoolWork(work);
}

static void win_threadpool_destroy(cleaner_threadpool_t *tp)
{
  if (!tp)
    return;

  CloseThreadpool(tp->pool);
  free(tp);
}

/* ========================================================= */
/* GLOBAL API TABLE (ORDER MUST MATCH HEADER EXACTLY)       */
/* ========================================================= */

static const cleaner_platform_api_t g_api = {

    /* ABI */
    CLEANER_PLATFORM_ABI_VERSION,

    /* File */
    win_file_open, win_file_write, win_file_flush, win_file_close,

    /* Directory */
    win_fs_mkdir, win_fs_rename, win_fs_walk,

    /* Time */
    win_time_now,

    /* Mutex */
    win_mutex_create, win_mutex_lock, win_mutex_unlock, win_mutex_destroy,

    /* Condition */
    win_cond_create, win_cond_wait, win_cond_signal, win_cond_broadcast,
    win_cond_destroy,

    /* Threadpool */
    win_threadpool_create, win_threadpool_submit, win_threadpool_destroy};

CLEANER_PLATFORM_API
const cleaner_platform_api_t *cleaner_platform_get_api(void) { return &g_api; }

#endif