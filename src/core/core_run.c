// src/core/core_run.c
// Main orchestration pipeline

#include "cleaner/core/core_run.h"
#include "cleaner/core/error.h"
#include "cleaner/core/processor.h"

#include "cleaner/system/file_scheduler.h"
#include "cleaner/system/fs_adapter.h"
#include "cleaner/system/tx_fs.h"

#include "cleaner/wal/wal.h"

#include <stdatomic.h>
#include <stdio.h>

/**
 * Main pipeline orchestration
 */
error_t core_run(const cleaner_platform_api_t *platform,
                 const cleaner_config_t *config)
{
  if (!platform || !config)
    return error_make(ERR_INTERNAL, "Invalid core_run parameters");

  if (!config->source)
    return error_make(ERR_INTERNAL, "Source path required");

  /* ========== FS Adapter ========== */
  fs_adapter_t adapter;
  if (fs_adapter_init(&adapter, platform) != 0)
    return error_make(ERR_INTERNAL, "FS adapter init failed");

  fs_interface_t fs_iface;
  fs_adapter_build_interface(&adapter, &fs_iface);

  /* ========== WAL ========== */
  cleaner_wal_t wal;
  if (cleaner_wal_open(&wal, "cleaner.wal") != 0)
    return error_make(ERR_INTERNAL, "WAL open failed");

  /* ========== TX FS ========== */
  tx_fs_t tx;
  if (tx_fs_init(&tx, &fs_iface, &wal) != 0)
    return error_make(ERR_INTERNAL, "TX FS init failed");

  if (tx_fs_recover(&tx) != 0)
    return error_make(ERR_INTERNAL, "TX FS recovery failed");

  fs_interface_t tx_iface;
  tx_fs_build_interface(&tx, &tx_iface);

  /* ========== Processor ========== */
  processor_t processor;
  if (processor_init(&processor, &tx_iface, NULL, config->dry_run) != 0)
    return error_make(ERR_INTERNAL, "Processor init failed");

  /* ========== File Scheduler ========== */
  file_scheduler_t scheduler = {.processor = &processor,
                                .executor = NULL,
                                .counter = 0,
                                .pending_tasks = 0};

  if (file_scheduler_run(&scheduler, config->source) != 0)
  {
    return error_make(ERR_WORK_FAILED, "File scheduler failed");
  }

  printf("[SUCCESS] Pipeline complete. Processed %llu files\n",
         (unsigned long long)atomic_load(&scheduler.counter));

  return error_ok();
}
