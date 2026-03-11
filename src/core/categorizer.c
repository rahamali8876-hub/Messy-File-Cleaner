// src/core/categorizer.c
// Pure business logic for file categorization (CORE layer - NO OS calls)

#include "cleaner/core/categorizer.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

/* Month names lookup table */
static const char *month_names[13] = {"unknown", // index 0 (invalid)
                                      "jan",     "feb", "mar", "apr",
                                      "may",     "jun", "jul", "aug",
                                      "sep",     "oct", "nov", "dec"};

const char *categorizer_month_name(int month) {
  if (month < 1 || month > 12)
    return month_names[0];
  return month_names[month];
}

/**
 * Extract and normalize extension
 * "photo.JPG" -> "jpg" (normalized to lowercase)
 * "file" -> "other"
 * "archive.tar.gz" -> "gz"
 */
int categorizer_extract_extension(const char *path, char *out_ext,
                                  size_t ext_size) {
  if (!path || !out_ext || ext_size < 8)
    return -1;

  // Find last dot
  const char *dot = strrchr(path, '.');

  if (!dot || dot == path) {
    // No extension found
    strncpy(out_ext, "other", ext_size - 1);
    out_ext[ext_size - 1] = '\0';
    return 0;
  }

  // Copy extension (skip the dot)
  const char *ext_start = dot + 1;
  size_t ext_len = strlen(ext_start);

  // Validate extension length
  if (ext_len == 0 || ext_len >= ext_size) {
    strncpy(out_ext, "other", ext_size - 1);
    out_ext[ext_size - 1] = '\0';
    return 0;
  }

  // Copy and normalize to lowercase
  for (size_t i = 0; i < ext_len && i < ext_size - 1; i++) {
    out_ext[i] = (char)tolower((unsigned char)ext_start[i]);
  }
  out_ext[ext_len] = '\0';

  // Reject invalid characters
  for (size_t i = 0; out_ext[i]; i++) {
    if (!isalnum((unsigned char)out_ext[i]) && out_ext[i] != '_' &&
        out_ext[i] != '-') {
      strcpy(out_ext, "other");
      return 0;
    }
  }

  return 0;
}

/**
 * Format category with date
 * "jpeg" + 12 + 2024 -> "organized/jpeg_dec_2024"
 */
int categorizer_format_category(const char *ext, int month, int year,
                                char *out_category, size_t cat_size) {
  if (!ext || !out_category || cat_size < 64)
    return -1;

  if (month < 1 || month > 12)
    return -1;

  if (year < 1970 || year > 2999)
    return -1;

  // Check extension length
  if (strlen(ext) > 50) {
    snprintf(out_category, cat_size, "organized/other");
    return 0;
  }

  const char *month_str = categorizer_month_name(month);

  int written = snprintf(out_category, cat_size, "organized/%s_%s_%d", ext,
                         month_str, year);

  if (written < 0 || (size_t)written >= cat_size)
    return -1;

  return 0;
}

/**
 * Extract month and year from time_t
 */
static int extract_date(time_t mod_time, int *out_month, int *out_year) {
  if (!out_month || !out_year)
    return -1;

  struct tm *time_info = localtime(&mod_time);
  if (!time_info)
    return -1;

  // month: 0-11 (convert to 1-12)
  *out_month = time_info->tm_mon + 1;
  // year: years since 1900 (convert to actual year)
  *out_year = time_info->tm_year + 1900;

  return 0;
}

/**
 * Full categorization pipeline
 */
int categorizer_categorize(const char *path, time_t mod_time,
                           file_category_t *out_cat) {
  if (!path || !out_cat)
    return -1;

  // Extract extension
  if (categorizer_extract_extension(path, out_cat->extension,
                                    sizeof(out_cat->extension)) != 0)
    return -1;

  // Extract date
  if (extract_date(mod_time, &out_cat->month, &out_cat->year) != 0)
    return -1;

  // Format category name
  if (categorizer_format_category(out_cat->extension, out_cat->month,
                                  out_cat->year, out_cat->category,
                                  sizeof(out_cat->category)) != 0)
    return -1;

  return 0;
}
