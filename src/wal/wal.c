// // src/wal/wal.c

#include "cleaner/wal/wal.h"
#include "cleaner/system/tx_fs.h"
#include <string.h>

int cleaner_wal_open(cleaner_wal_t *wal, const char *path)
{
  wal->file = fopen(path, "a+");

  if (!wal->file)
    return -1;

  return 0;
}

void cleaner_wal_close(cleaner_wal_t *wal)
{
  if (wal->file)
    fclose(wal->file);
}

void cleaner_wal_commit(cleaner_wal_t *wal) { fflush(wal->file); }

/* ================= LOG OPS ================= */

void cleaner_wal_log_move(cleaner_wal_t *wal, const char *src,
                          const char *dst)
{
  fprintf(wal->file, "MOVE %s %s\n", src, dst);
}

void cleaner_wal_log_delete(cleaner_wal_t *wal, const char *path)
{
  fprintf(wal->file, "DELETE %s\n", path);
}

void cleaner_wal_log_mkdir(cleaner_wal_t *wal, const char *path)
{
  fprintf(wal->file, "MKDIR %s\n", path);
}

FILE *cleaner_wal_file(cleaner_wal_t *wal)
{
  if (!wal)
    return NULL;

  return wal->file;
}

/* ================= RECOVERY ================= */

int cleaner_wal_replay(cleaner_wal_t *wal, fs_interface_t *fs)
{
  rewind(wal->file);

  char op[32];
  char a[512];
  char b[512];

  while (fscanf(wal->file, "%31s", op) == 1)
  {
    if (strcmp(op, "MOVE") == 0)
    {
      fscanf(wal->file, "%511s %511s", a, b);
      // fs->move(a, b);
      fs->move_file(fs->context, a, b);
    }

    else if (strcmp(op, "DELETE") == 0)
    {
      fscanf(wal->file, "%511s", a);
      fs->remove_file(fs->context, a);
    }

    else if (strcmp(op, "MKDIR") == 0)
    {
      fscanf(wal->file, "%511s", a);
      // fs->mkdir(a);
      fs->create_directory(fs->context, a);
      // fs->mkdir(a);
    }
  }

  return 0;
}