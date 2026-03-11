#include "cleaner/wal/wal.h"

#include <stdio.h>
#include <string.h>

#define WAL_LINE_MAX 1024

/* -------------------------------------------------- */
/* trim newline                                       */
/* -------------------------------------------------- */

static void trim_newline(char *s)
{
  if (!s)
    return;

  size_t len = strlen(s);

  if (len && s[len - 1] == '\n')
    s[len - 1] = '\0';
}

/* -------------------------------------------------- */
/* MOVE                                               */
/* -------------------------------------------------- */

static int replay_move(fs_interface_t *fs, const char *line)
{
  char src[512];
  char dst[512];

  if (sscanf(line, "MOVE %511s %511s", src, dst) != 2)
    return -1;

  return fs->move_file(fs->context, src, dst);
}

/* -------------------------------------------------- */
/* DELETE                                             */
/* -------------------------------------------------- */

static int replay_delete(fs_interface_t *fs, const char *line)
{
  char path[512];

  if (sscanf(line, "DELETE %511s", path) != 1)
    return -1;

  return fs->remove_file(fs->context, path);
}

/* -------------------------------------------------- */
/* MKDIR                                              */
/* -------------------------------------------------- */

static int replay_mkdir(fs_interface_t *fs, const char *line)
{
  char path[512];

  if (sscanf(line, "MKDIR %511s", path) != 1)
    return -1;

  return fs->create_directory(fs->context, path);
}

/* -------------------------------------------------- */
/* dispatcher                                         */
/* -------------------------------------------------- */

static int replay_operation(fs_interface_t *fs, const char *line)
{
  if (strncmp(line, "MOVE ", 5) == 0)
    return replay_move(fs, line);

  if (strncmp(line, "DELETE ", 7) == 0)
    return replay_delete(fs, line);

  if (strncmp(line, "MKDIR ", 6) == 0)
    return replay_mkdir(fs, line);

  return 0;
}

/* -------------------------------------------------- */
/* WAL replay                                         */
/* -------------------------------------------------- */

int cleaner_wal_replay(cleaner_wal_t *wal, fs_interface_t *fs)
{
  if (!wal || !wal->file || !fs)
    return -1;

  rewind(wal->file);

  char line[WAL_LINE_MAX];

  int in_tx = 0;
  int committed = 0;

  while (fgets(line, sizeof(line), wal->file))
  {
    trim_newline(line);

    if (strcmp(line, "BEGIN") == 0)
    {
      in_tx = 1;
      committed = 0;
      continue;
    }

    if (strcmp(line, "COMMIT") == 0)
    {
      committed = 1;
      in_tx = 0;
      continue;
    }

    if (!in_tx && !committed)
      continue;

    if (replay_operation(fs, line) != 0)
      return -1;
  }

  return 0;
}