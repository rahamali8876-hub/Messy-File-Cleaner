// #ifndef CLEANER_WORK_QUEUE_H
// #define CLEANER_WORK_QUEUE_H

// #include <stddef.h>

// #define WORK_QUEUE_MAX 8192

// typedef struct {
//   char paths[WORK_QUEUE_MAX][512];
//   size_t head;
//   size_t tail;
// } work_queue_t;

// void work_queue_init(work_queue_t *q);

// int work_queue_push(work_queue_t *q, const char *path);

// int work_queue_pop(work_queue_t *q, char *out);

// #endif

#ifndef CLEANER_WORK_QUEUE_H
#define CLEANER_WORK_QUEUE_H

#include <stddef.h>

#define WORK_QUEUE_MAX 8192
#define WORK_QUEUE_PATH_MAX 512

typedef struct {
  char paths[WORK_QUEUE_MAX][WORK_QUEUE_PATH_MAX];
  size_t head;
  size_t tail;
} work_queue_t;

void work_queue_init(work_queue_t *q);

int work_queue_push(work_queue_t *q, const char *path);

int work_queue_pop(work_queue_t *q, char *out);

#endif