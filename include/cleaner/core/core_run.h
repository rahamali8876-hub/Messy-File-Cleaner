// // include/cleaner/core/core_run.h

#ifndef CLEANER_CORE_CORE_RUN_H
#define CLEANER_CORE_CORE_RUN_H

#include "cleaner/platform/platform_api.h"
#include "cleaner/core/config.h"
#include "cleaner/core/error.h"

error_t core_run(const cleaner_platform_api_t *platform,
                 const cleaner_config_t *config);

#endif