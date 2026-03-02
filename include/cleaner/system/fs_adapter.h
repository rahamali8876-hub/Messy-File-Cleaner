// include/cleaner/system/fs_adapter.h

#ifndef CLEANER_SYSTEM_FS_ADAPTER_H
#define CLEANER_SYSTEM_FS_ADAPTER_H

// #include "platform_api.h"
// #include "fs_interface.h"
#include "cleaner/platform/platform_api.h"
#include "cleaner/core/fs_interface.h"

typedef struct
{
    const cleaner_platform_api_t *platform;
} fs_adapter_t;

/* Initialize the adapter */
int fs_adapter_init(fs_adapter_t *adapter,
                    const cleaner_platform_api_t *platform);

/* Build core fs interface from adapter */
void fs_adapter_build_interface(fs_adapter_t *adapter,
                                fs_interface_t *out);

#endif