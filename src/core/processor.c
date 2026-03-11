// cleaner/src/core/processor.c

#include "cleaner/core/processor.h"
#include "cleaner/core/categorizer.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

#define FS_CALL(fn)                                                            \
  do {                                                                         \
    if (!(fn)) {                                                               \
      fprintf(stderr, "FS interface missing: %s\n", #fn);                      \
      return -1;                                                               \
    }                                                                          \
  } while (0)

/**
 * Process single file: categorize and move
 * Uses CORE layer pure categorization logic
 */
int processor_process_file(processor_t *processor, const fs_entry_t *entry,
                           uint64_t id) {
  if (!processor || !entry)
    return -1;

  printf("[Processor] id=%llu path=%s\n", (unsigned long long)id,
         entry->path ? entry->path : "(null)");

  if (entry->is_directory)
    return 0;

  FS_CALL(processor->fs.create_directory);
  FS_CALL(processor->fs.move_file);

  // Use CORE layer categorization (pure business logic)
  file_category_t category;

  time_t mod_time = entry->modification_time;
  if (mod_time == 0)
    mod_time = time(NULL); // Use current time if not available

  if (categorizer_categorize(entry->path, mod_time, &category) != 0) {
    fprintf(stderr, "Categorization failed for: %s\n", entry->path);
    return -1;
  }

  // Build target path: organized/ext_month_year/filename
  char target_path[2048];

  // Extract filename from path
  const char *filename = strrchr(entry->path, '\\');
  if (!filename)
    filename = strrchr(entry->path, '/');
  if (!filename)
    filename = entry->path;
  else
    filename++;

  // Validate filename length
  if (strlen(filename) > 500) {
    fprintf(stderr, "Filename too long: %s\n", filename);
    return -1;
  }

  // Build full target path
  int target_result = snprintf(target_path, sizeof(target_path), "%s/%s",
                               category.category, filename);

  if (target_result < 0 || (size_t)target_result >= sizeof(target_path)) {
    fprintf(stderr, "Target path too long\n");
    return -1;
  }

  if (processor->dry_run) {
    printf("[DryRun] mkdir %s\n", category.category);
    printf("[DryRun] move %s -> %s\n", entry->path, target_path);
    return 0;
  }

  // Create directory
  if (processor->fs.create_directory(processor->fs.context,
                                     category.category) != 0) {
    fprintf(stderr, "Directory create failed: %s\n", category.category);
    return -1;
  }

  // Move file
  if (processor->fs.move_file(processor->fs.context, entry->path,
                              target_path) != 0) {
    fprintf(stderr, "Move failed: %s -> %s\n", entry->path, target_path);
    return -1;
  }

  return 0;
}

int processor_init(processor_t *p, const fs_interface_t *fs,
                   int (*get_time_fn)(renamer_timestamp_t *), int dry_run) {
  if (!p || !fs)
    return -1;

  memset(p, 0, sizeof(*p));

  p->fs = *fs;
  p->get_time = get_time_fn;
  p->dry_run = dry_run;

  return 0;
}