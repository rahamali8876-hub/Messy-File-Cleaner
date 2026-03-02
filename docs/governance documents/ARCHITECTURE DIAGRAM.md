# ARCHITECTURE DIAGRAM

This document defines the official architectural model of the Cleaner project.

---

# Layered Architecture

```
            +------------------+
            |       CLI        |
            +------------------+
                      |
                      v
            +------------------+
            |      SYSTEM      |
            | (Orchestration)  |
            +------------------+
                |            |
                v            v
          +-----------+   +-----------+
          |   CORE    |   | PLATFORM  |
          | (Domain)  |   | (OS Wrap) |
          +-----------+   +-----------+
                                   |
                                   v
                              +---------+
                              |   OS    |
                              +---------+
```

---

# Dependency Rules

    Allowed:

    CLI → System
    System → Core
    System → Platform
    Platform → OS

    Forbidden:

    Core → System
    Core → Platform
    System → OS
    Platform → Core
    Platform → System

    No circular dependencies allowed.

---

# Responsibility Breakdown

## CLI

    * Parses arguments
    * Triggers workflows
    * No business logic

## Core

    * Business rules
    * Deterministic algorithms
    * No side effects

## System

    * Scheduling
    * Logging coordination
    * Metrics coordination
    * Thread pool management

## Platform

    * Filesystem wrappers
    * Thread primitives
    * Mutex primitives
    * Time abstraction
    * OS-specific implementations

---

# Golden Rule

    Core must compile and run without linking to Platform or OS.

    If Core cannot be compiled independently, architecture is broken.

---

### Evolution Rule

    When adding new features:

    1. Ask: Is this domain logic? → Core
    2. Ask: Is this coordination? → System
    3. Ask: Is this OS-specific? → Platform

    If unclear, do not implement until clarified.

    Architecture stability is more important than speed of development.


🧠 Now Let Me Elevate This One Level

Here is your correct dependency graph:

          +-----------+
          |   Core    |
          | (Pure)    |
          +-----------+
                ^
                |
          +-----------+
          |  System   |
          | Orchestr. |
          +-----------+
                ^
                |
          +-----------+
          | Platform  |
          |  (OS)     |
          +-----------+

    Rules:

    Core → depends on nothing

    System → depends on Core

    Platform → depends on OS only

    System calls Platform

    System calls Core

    Platform never calls Core

    Core never calls Platform

    If you violate this once,
    your architecture collapses slowly over time.