// cleaner/src/core/processor.c

// #include "cleaner/core/processor.h"
// #include <stdio.h>

// int processor_process_file(processor_t *processor, const fs_entry_t *entry,
//                            uint64_t id) {
//   if (!processor || !entry)
//     return -1;

//   printf("[Processor] id=%llu path=%s\n", (unsigned long long)id, entry->path);

//   // TODO: replace with real logic: scan, rename, move
//   return 0;
// }

// int processor_init(processor_t *p, const fs_interface_t *fs,
//                    int (*get_time_fn)(renamer_timestamp_t *), int dry_run) {
//   if (!p || !fs)
//     return -1;

//   p->fs = *fs;
//   p->get_time = get_time_fn;
//   p->dry_run = dry_run;
//   return 0;
// }

#include "cleaner/core/processor.h"
#include <stdio.h>
#include <string.h>

#define FS_CALL(fn)                         \
  do                                        \
  {                                         \
    if (!(fn))                              \
    {                                       \
      fprintf(stderr,                       \
              "FS interface missing: %s\n", \
              #fn);                         \
      return -1;                            \
    }                                       \
  } while (0)

int processor_process_file(processor_t *processor,
                           const fs_entry_t *entry,
                           uint64_t id)
{
  if (!processor || !entry)
    return -1;

  printf("[Processor] id=%llu path=%s\n",
         (unsigned long long)id,
         entry->path);

  if (entry->is_directory)
    return 0;

  /* validate interface */
  FS_CALL(processor->fs.create_directory);
  FS_CALL(processor->fs.move);

  char dir_path[512];
  char target_path[1024];

  const char *ext = strrchr(entry->path, '.');

  if (!ext)
    ext = ".other";

  snprintf(dir_path,
           sizeof(dir_path),
           "organized/%s",
           ext + 1);

  const char *filename = strrchr(entry->path, '\\');

  if (!filename)
    filename = strrchr(entry->path, '/');

  if (!filename)
    filename = entry->path;
  else
    filename++;

  snprintf(target_path,
           sizeof(target_path),
           "%s/%s",
           dir_path,
           filename);

  if (processor->dry_run)
  {
    printf("[DryRun] mkdir %s\n", dir_path);
    printf("[DryRun] move %s -> %s\n",
           entry->path,
           target_path);
    return 0;
  }

  if (processor->fs.create_directory(processor->fs.context,
                                     dir_path) != 0)
  {
    fprintf(stderr,
            "Directory create failed: %s\n",
            dir_path);
    return -1;
  }

  if (processor->fs.move(entry->path,
                         target_path) != 0)
  {
    fprintf(stderr,
            "Move failed: %s -> %s\n",
            entry->path,
            target_path);
    return -1;
  }

  return 0;
}

int processor_init(processor_t *p,
                   const fs_interface_t *fs,
                   int (*get_time_fn)(renamer_timestamp_t *),
                   int dry_run)
{
  if (!p || !fs)
    return -1;

  memset(p, 0, sizeof(*p));

  p->fs = *fs;
  p->get_time = get_time_fn;
  p->dry_run = dry_run;

  return 0;
}