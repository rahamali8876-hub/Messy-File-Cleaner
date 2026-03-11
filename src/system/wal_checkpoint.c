// // src/system/wal_checkpoint.c

#include "cleaner/system/wal_checkpoint.h"
#include "cleaner/wal/wal.h"
#include "cleaner/wal/wal_format.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <io.h> /* for _commit */
#endif

/* Internal WAL checkpoint struct */
struct wal_checkpoint
{
  cleaner_wal_t *wal;
  char *path; /* snapshot file path */
};

/* Initialize checkpoint engine */
int wal_checkpoint_init(wal_checkpoint_t **out, cleaner_wal_t *wal,
                        const char *checkpoint_path)
{
  if (!out || !wal || !checkpoint_path)
    return -1;

  wal_checkpoint_t *cp = calloc(1, sizeof(*cp));
  if (!cp)
    return -1;

  cp->wal = wal;
  cp->path = strdup(checkpoint_path);
  if (!cp->path)
  {
    free(cp);
    return -1;
  }

  *out = cp;
  return 0;
}

/* Run checkpoint: flush WAL to snapshot file */
int wal_checkpoint_run(wal_checkpoint_t *cp)
{
  if (!cp || !cp->wal || !cp->path)
    return -1;

  FILE *src = cleaner_wal_file(cp->wal);
  if (!src)
    return -1;

  FILE *dst = fopen(cp->path, "wb");
  if (!dst)
    return -1;

  /* Flush WAL first */
  fflush(src);
#if defined(_WIN32)
  _commit(_fileno(src));
#else
  fsync(fileno(src));
#endif

  /* Copy WAL contents atomically */
  rewind(src);
  char buffer[4096];
  size_t n;
  while ((n = fread(buffer, 1, sizeof(buffer), src)) > 0)
  {
    if (fwrite(buffer, 1, n, dst) != n)
    {
      fclose(dst);
      return -1;
    }
  }

  fflush(dst);
#if defined(_WIN32)
  _commit(_fileno(dst));
#else
  fsync(fileno(dst));
#endif

  fclose(dst);
  fseek(src, 0, SEEK_END); /* Restore WAL position */

  return 0;
}

/* Destroy checkpoint engine */
int wal_checkpoint_destroy(wal_checkpoint_t *cp)
{
  if (!cp)
    return 0;

  free(cp->path);
  free(cp);
  return 0;
}