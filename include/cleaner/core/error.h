// cleaner/core/error.h

#ifndef CLEANER_CORE_ERROR_H
#define CLEANER_CORE_ERROR_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  ERR_OK = 0,
  ERR_CANCELLED,
  ERR_QUEUE_FULL,
  ERR_WORK_FAILED,
  ERR_INTERNAL
} error_code_t;

typedef struct {
  error_code_t code;
  const char *message; /* static string */
} error_t;

/* Constructors */
error_t error_ok(void);
error_t error_make(error_code_t code, const char *msg);

/* Helpers */
int error_is_ok(error_t err);
const char *error_string(error_code_t code);

#ifdef __cplusplus
}
#endif

#endif