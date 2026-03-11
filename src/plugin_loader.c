#include "cleaner/plugin_loader.h"
#include <windows.h>

int plugin_load(const char *path, loaded_plugin_t *out)
{
  HMODULE lib = LoadLibraryA(path);

  if (!lib)
    return -1;

  cleaner_fs_plugin_create_fn create =
      (cleaner_fs_plugin_create_fn)GetProcAddress(lib,
                                                  "cleaner_fs_plugin_create");

  if (!create)
  {
    FreeLibrary(lib);
    return -1;
  }

  cleaner_fs_plugin_t *plugin = create();

  out->handle = lib;
  out->plugin = plugin;

  return 0;
}