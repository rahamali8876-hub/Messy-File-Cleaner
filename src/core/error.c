// src/core/error.c
#include "cleaner/core/error.h"

error_t error_ok(void) {
  error_t e;
  e.code = ERR_OK;
  e.message = "OK";
  return e;
}

error_t error_make(error_code_t code, const char *msg) {
  error_t e;
  e.code = code;
  e.message = msg;
  return e;
}

int error_is_ok(error_t err) { return err.code == ERR_OK; }

const char *error_string(error_code_t code) {
  switch (code) {
  case ERR_OK:
    return "OK";
  case ERR_CANCELLED:
    return "Cancelled";
  case ERR_QUEUE_FULL:
    return "Queue Full";
  case ERR_WORK_FAILED:
    return "Work Failed";
  case ERR_INTERNAL:
    return "Internal Error";
  default:
    return "Unknown Error";
  }
}