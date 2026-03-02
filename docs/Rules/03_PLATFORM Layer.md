# PLATFORM Layer Rules

## Purpose

    The Platform layer isolates operating system details. It provides a stable abstraction over OS APIs.

    Platform is the ONLY layer allowed to interact with the operating system.

---

## Responsibilities

    Platform MUST:

    * Wrap OS filesystem APIs
    * Wrap OS thread creation
    * Wrap mutex and atomic primitives
    * Wrap time functions
    * Convert OS data structures into portable representations

    Platform implements mechanism, not policy.

---

## Absolute Restrictions

    Platform MUST NOT:

    * Contain business rules
    * Contain domain logic
    * Contain scheduling policy
    * Contain logging policy
    * Contain metrics policy

    Platform is a thin translation layer.

---

## Dependency Rules

    Allowed:

    Platform → OS

    Forbidden:

    Platform → Core
    Platform → System
    Core → Platform (directly)

    Platform must never depend on higher layers.

---

## Design Principles

    * Thin wrappers
    * Minimal logic
    * No static global policies
    * Clear naming (cleaner_fs_*, cleaner_thread_*)
    * Each API must have OS-specific implementations

---

## File Structure Rules

    Each public header in include/cleaner/platform/ must:

    * Define a clean, portable interface
    * Avoid exposing OS-specific types
    * Avoid leaking HANDLE, FILETIME, pthread_t, etc.

    Each OS implementation must be isolated:

    * filesystem_win.c
    * filesystem_posix.c
    * threads_win.c
    * threads_posix.c

---

## Violation Examples

    * Implementing extension logic here
    * Implementing thread pool logic here
    * Making scheduling decisions here
    * Including Core headers

    These break layer direction.

---

## Review Checklist

    Before merging Platform code, confirm:

    * Only OS interaction exists here
    * No business logic
    * No orchestration logic
    * Interface is OS-agnostic
    * Higher layers are not referenced

    If violated → reject immediately.

---

## Architecture Chain (Final Law)

    CLI → System → Core
    ↓
    Platform
    ↓
    OS

    No reverse dependencies allowed.


### Final Stability Rules
    Platform exposes only platform_api.h

    No platform header included outside platform/

    No struct layout changes without ABI bump

    ABI version must increment on signature change

    All ownership rules documented

    No OS types in public headers