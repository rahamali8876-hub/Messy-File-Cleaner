
// cleaner/src/core/organizer.c

#include "cleaner/core/organizer.h"
#include <ctype.h>
#include <string.h>

static void to_lowercase(const char *src, char *dest, size_t size) {
  size_t i;

  for (i = 0; src[i] && i < size - 1; i++)
    dest[i] = (char)tolower((unsigned char)src[i]);

  dest[i] = '\0';
}

int get_extension_folder(const char *filename, char *out, size_t out_size) {
  if (!filename || !out || out_size == 0)
    return -1;

  const char *dot = strrchr(filename, '.');

  if (!dot || dot == filename)
    return -1;

  to_lowercase(dot + 1, out, out_size);

  return 0;
}