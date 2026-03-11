#include "cleaner/core/path_utils.h"

#include <stdio.h>
#include <string.h>

int path_create_recursive(fs_interface_t *fs, const char *path) {
  char tmp[512];
  size_t len;

  if (!fs || !path)
    return -1;

  len = strlen(path);

  if (len >= sizeof(tmp))
    return -1;

  strcpy(tmp, path);

  for (size_t i = 1; i < len; ++i) {
    if (tmp[i] == '/' || tmp[i] == '\\') {
      char c = tmp[i];
      tmp[i] = '\0';

      fs->create_directory(fs->context, tmp);

      tmp[i] = c;
    }
  }

  return fs->create_directory(fs->context, tmp);
}