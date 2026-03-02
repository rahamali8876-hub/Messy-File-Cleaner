// cleaner/include\organizer.h

#ifndef CLEANER_CORE_ORGANIZER_H
#define CLEANER_CORE_ORGANIZER_H

#include <stddef.h>

/*
 * Extract lowercase extension.
 *
 * Returns:
 *   0 on success
 *  -1 if no extension
 */
int get_extension_folder(
    const char *filename,
    char *out,
    size_t out_size);

#endif