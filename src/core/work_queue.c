// #include "cleaner/core/work_queue.h"
// #include <string.h>

// void work_queue_init(work_queue_t *q)
// {
//   q->head = 0;
//   q->tail = 0;
// }

// int work_queue_push(work_queue_t *q, const char *path)
// {
//   if ((q->tail + 1) % WORK_QUEUE_MAX == q->head)
//     return -1;

//   // strcpy(q->paths[q->tail], path);
//   // q->tail = (q->tail + 1) % WORK_QUEUE_MAX;

//   strncpy(q->paths[q->tail], path, MAX_PATH - 1);
//   q->paths[q->tail][MAX_PATH - 1] = '\0';

//   return 0;
// }

// int work_queue_pop(work_queue_t *q, char *out)
// {
//   if (q->head == q->tail)
//     return -1;

//   strcpy(out, q->paths[q->head]);
//   q->head = (q->head + 1) % WORK_QUEUE_MAX;

//   return 0;
// }

#include "cleaner/core/work_queue.h"
#include <string.h>

void work_queue_init(work_queue_t *q) {
  q->head = 0;
  q->tail = 0;
}

int work_queue_push(work_queue_t *q, const char *path) {
  if ((q->tail + 1) % WORK_QUEUE_MAX == q->head)
    return -1;

  strncpy(q->paths[q->tail], path, WORK_QUEUE_PATH_MAX - 1);
  q->paths[q->tail][WORK_QUEUE_PATH_MAX - 1] = '\0';

  q->tail = (q->tail + 1) % WORK_QUEUE_MAX;

  return 0;
}

int work_queue_pop(work_queue_t *q, char *out) {
  if (q->head == q->tail)
    return -1;

  // strcpy(out, q->paths[q->head]);
  strncpy(out, q->paths[q->head], WORK_QUEUE_PATH_MAX);
  q->head = (q->head + 1) % WORK_QUEUE_MAX;

  return 0;
}