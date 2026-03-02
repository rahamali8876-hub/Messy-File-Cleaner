# SYSTEM Layer Rules

## Purpose

    The System layer coordinates infrastructure. It implements orchestration, scheduling, logging, metrics, and application-level services.

    System contains policy and coordination logic, but NEVER operating system implementations.

---

## Responsibilities

    System MAY:

    * Implement thread pools
    * Implement logging abstractions
    * Implement metrics collection
    * Coordinate Core workflows
    * Manage lifecycle and shutdown
    * Translate CLI input into Core operations

    System defines HOW things run, not HOW the OS performs them.

---

## Absolute Restrictions

    System MUST NOT:

    * Include OS headers (windows.h, pthread.h, etc.)
    * Call OS APIs directly
    * Contain platform-specific code
    * Implement filesystem syscalls
    * Implement thread primitives

    If System includes windows.h or similar → immediate rejection.

---

## Dependency Rules

    Allowed:

    System → Core
    System → Platform

    Forbidden:

    System → OS
    Core → System
    Platform → System

    System sits between Core and Platform.

---

## Design Principles

    * Infrastructure orchestration
    * Clear separation of policy vs mechanism
    * Uses platform abstractions only
    * No hardcoded OS assumptions
    * Thread-safe if required (via platform abstractions)

---

## Example Correct Patterns

    ✔ ThreadPool uses cleaner_thread_create()
    ✔ Logger uses cleaner_mutex_lock()
    ✔ Metrics uses cleaner_time_now_ns()

---

## Violation Examples

    * Using CreateMutex directly
    * Using GetLocalTime directly
    * Using MoveFile directly
    * Including windows.h in system/logger.c

    These are platform responsibilities.

---

## Review Checklist

    Before merging System code, confirm:

    * No OS includes
    * Only platform abstractions used
    * No domain logic leaking from Core
    * Clear separation of coordination vs mechanism

    If violated → reject the change.
