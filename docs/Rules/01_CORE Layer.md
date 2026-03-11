# CORE Layer Rules

## Purpose

    The Core layer contains pure domain logic. It represents the business rules and deterministic behavior of the application.

    Core must be portable, testable, and independent of operating systems, threading, filesystem access, or infrastructure concerns.

---

## Absolute Restrictions (NON‑NEGOTIABLE)

    Core MUST NOT:

    * Include OS headers (windows.h, pthread.h, unistd.h, etc.)
    * Include platform headers
    * Include system headers
    * Perform file I/O
    * Spawn threads
    * Use mutexes or atomics
    * Call time APIs
    * Allocate OS resources
    * Log directly to files

    If any of the above appears in Core, it is an architectural violation.

---

### What Core IS Allowed To Do

    Core MAY:

    * Manipulate strings
    * Use standard C library headers (string.h, stdlib.h, stdint.h, etc.)
    * Implement algorithms
    * Define domain data structures
    * Apply business rules
    * Transform data
    * Return deterministic results

    Core must behave the same regardless of operating system.

---

## Dependency Rules

    Allowed:

    Core → Nothing

    Forbidden:

    Core → System
    Core → Platform
    Core → OS

    Core is the most protected layer.

---

## Design Principles

    * Deterministic behavior
    * No side effects beyond function scope
    * Pure input → output transformation where possible
    * Fully unit-testable without mocks of OS
    * Stable APIs

---

## Examples of Valid Core Code

    * File extension parsing
    * Name normalization
    * Sorting logic
    * Rule engines
    * Decision trees

---

## Violation Examples

    * Including platform/filesystem.h
    * Calling cleaner_fs_walk()
    * Using GetFileAttributes()
    * Using pthread_mutex_lock()

    These break isolation and must be rejected during code review.

---

## Review Checklist

    Before merging Core code, confirm:

    * No OS includes
    * No platform includes
    * No system includes
    * No global mutable state
    * Unit tests exist

    If any answer is NO → reject the change.

### ✅ Summary Table

    Layer Can Talk To Reason
    Core None Must stay pure, deterministic, OS-agnostic
    System Core, Platform Orchestration layer: defines what to do, uses Core (logic) and Platform (mechanism)
    Platform OS only Thin wrappers over OS; no knowledge of business logic

    Visual (Enterprise Chain):

    CLI / App
    ↓
    System  → Core (business rules)
            → Platform (OS wrappers)
    ↓
    Platform → OS

    Notice all arrows go down, no reverse dependency.
