// include/cleaner/core/categorizer.h
// Pure business logic for file categorization

#ifndef CLEANER_CORE_CATEGORIZER_H
#define CLEANER_CORE_CATEGORIZER_H

#include <stddef.h>
#include <stdint.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * File category with extension and date info
 * Example: jpeg_dec_2024, pdf_jan_2025, etc.
 */
typedef struct {
  char extension[64]; // "jpeg", "pdf", "txt"
  char category[256]; // "jpeg_dec_2024"
  int month;          // 1-12
  int year;           // 2024
} file_category_t;

/**
 * Extract file extension (business logic, pure C)
 *
 * @param path: file path
 * @param out_ext: output extension buffer (min 64 bytes)
 * @return: 0 on success, -1 on error
 *
 * Examples:
 *   "photo.jpg" -> "jpg"
 *   "document.JPEG" -> "jpeg" (normalized)
 *   "file" -> "other" (no extension)
 */
int categorizer_extract_extension(const char *path, char *out_ext,
                                  size_t ext_size);

/**
 * Format category name with date
 *
 * @param ext: extension (e.g., "jpeg")
 * @param month: file modification month (1-12)
 * @param year: file modification year (e.g., 2024)
 * @param out_category: output category buffer (min 256 bytes)
 * @return: 0 on success, -1 on error
 *
 * Examples:
 *   ext="jpeg", month=12, year=2024 -> out="organized/jpeg_dec_2024"
 *   ext="pdf", month=1, year=2025 -> out="organized/pdf_jan_2025"
 */
int categorizer_format_category(const char *ext, int month, int year,
                                char *out_category, size_t cat_size);

/**
 * Full categorization pipeline (business logic only)
 *
 * @param path: file path
 * @param mod_time: file modification time (time_t)
 * @param out_cat: output category
 * @return: 0 on success, -1 on error
 */
int categorizer_categorize(const char *path, time_t mod_time,
                           file_category_t *out_cat);

/**
 * Month name lookup (pure data)
 */
const char *categorizer_month_name(int month);

#ifdef __cplusplus
}
#endif

#endif // CLEANER_CORE_CATEGORIZER_H
