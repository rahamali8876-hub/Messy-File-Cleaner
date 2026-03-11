#ifndef CLEANER_CORE_EXECUTOR_INTERFACE_H
#define CLEANER_CORE_EXECUTOR_INTERFACE_H

typedef void (*executor_task_fn)(void *arg);

typedef struct executor_interface {

  void (*submit)(executor_task_fn fn, void *arg);

  void (*wait_all)(void);

  void (*shutdown)(void);

} executor_interface_t;

#endif