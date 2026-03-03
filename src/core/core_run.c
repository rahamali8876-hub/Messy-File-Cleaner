// src/core/core_run.c
#include "cleaner/core/core_run.h"
#include "cleaner/core/error.h"
#include "cleaner/core/processor.h"
#include "cleaner/system/executor.h"
#include "cleaner/system/file_scheduler.h"
#include "cleaner/system/fs_adapter.h"

error_t core_run(const cleaner_platform_api_t *platform,
                 const cleaner_config_t *config) {
  if (!platform || !config)
    return error_make(ERR_INTERNAL, "Invalid core_run parameters");

  fs_adapter_t adapter;

  if (fs_adapter_init(&adapter, platform) != 0)

    return error_make(ERR_INTERNAL, "FS adapter init failed");

  fs_interface_t fs_iface;
  fs_adapter_build_interface(&adapter, &fs_iface);

  processor_t processor;
  processor_init(&processor, &fs_iface, NULL, config->dry_run);

  executor_interface_t *executor = NULL;
  // error_t err = executor_create(platform, 4, &executor);
  error_t err = executor_create(platform, 1, &executor);
  if (!error_is_ok(err))
    return err;

  file_scheduler_t scheduler = {
      .processor = &processor, .executor = executor, .counter = 0};

  // int sched_result = file_scheduler_run(&scheduler, config->source);
  error_t sched_err = file_scheduler_run(&scheduler, config->source);

  executor_destroy(executor);

  if (!error_is_ok(sched_err))
    return sched_err;

  // if (sched_result != 0)
  //     return error_make(ERR_WORK_FAILED, "Scheduler failed");

  return error_ok();
}