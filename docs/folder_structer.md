
cleat's create entriprise level project with c programming , listen carefully what i want
### 🏗 PRINCIPAL-GRADE STRUCTURE (Upgraded)
cleaner/
│
├── CMakeLists.txt
├── cmake/
│   └── CompilerWarnings.cmake
│
├── include/
│   └── cleaner/
│
│       ├── config.h
│       │
│       ├── core/                      # 🧠 PURE DOMAIN LAYER
│       │   ├── processor.h
│       │   ├── scanner.h
│       │   ├── organizer.h
│       │   ├── renamer.h
│       │   ├── fs_interface.h         # Abstract filesystem contract
│       │   ├── executor_interface.h   # Abstract execution contract
│       │   ├── task_context.h
│       │   └── error.h
│       │
│       ├── system/                    # 🔧 INFRASTRUCTURE / ADAPTER LAYER
│       │   ├── bounded_queue.h
│       │   ├── retry.h
│       │   ├── logger.h
│       │   ├── metrics.h
│       │   ├── fs_adapter.h           # Implements core fs_interface
│       │   ├── executor_adapter.h     # Implements executor_interface
│       │   └── time_adapter.h
│       │   ├── wal.h                  # crash-safe logging
│       │   └── memory_pool.h          # optional arena
                recovery.h             # optional WAL-based recovery
│       │
│       └── platform/                  # 🖥 PLATFORM ABSTRACTION (OS Layer)
│           ├── filesystem/
│           │   ├── filesystem_win.c
│           │   └── filesystem_posix.c
│           │
│           ├── threads/
│           │   ├── threads_win.c
│           │   └── threads_posix.c
│           │
│           ├── sync/
│           │   ├── mutex_win.c
│           │   ├── mutex_posix.c
│           │   ├── atomic_win.c
│           │   └── atomic_posix.c
│           │
│           ├── threadpool/
│           │   ├── threadpool_win.c
│           │   └── threadpool_posix.c
│           │
│           ├── windows/
│           │   └── file_win.c
│           │
│           ├── time/
│           │   ├── time_win.c
│           │   └── time_posix.c
│           │
│           └── platform_api.c         # Unified OS ABI table
│
├── src/
│
│   ├── core/                          # 🧠 Domain Implementation
│   │   ├── processor.c
│   │   ├── scanner.c
│   │   ├── organizer.c
│   │   ├── renamer.c
│   │   ├── task_context.c
│   │   └── error.c
│   │
│   ├── system/                        # 🔧 Infrastructure Implementation
│   │   ├── bounded_queue.c
│   │   ├── retry.c
│   │   ├── logger.c
│   │   ├── metrics.c
│   │   ├── fs_adapter.c
│   │   ├── executor_adapter.c
│   │   └── time_adapter.c
│   │   ├── wal.c                        # crash-safe logging
│   │   └── memory_pool.c          # optional arena
            recovery.c             # optional WAL-based recovery
│   │
│   ├── platform/                      # 🖥 OS-Specific Implementation
│   │   ├── filesystem/
│   │   │   ├── filesystem_win.c
│   │   │   └── filesystem_posix.c
│   │   │
│   │   ├── threads/
│   │   │   ├── threads_win.c
│   │   │   └── threads_posix.c
│   │   │
│   │   ├── sync/
│   │   │   ├── mutex_win.c
│   │   │   ├── mutex_posix.c
│   │   │   ├── atomic_win.c
│   │   │   └── atomic_posix.c
│   │   │
│   │   ├── threadpool/
│   │   │   ├── threadpool_win.c
│   │   │   └── threadpool_posix.c
│   │   │
│   │   ├── windows/
│   │   │   └── file_win.c
│   │   │
│   │   ├── time/
│   │   │   ├── time_win.c
│   │   │   └── time_posix.c
│   │   │
│   │   └── platform_win.c             # Windows ABI binding
│   │
│   └── app/
│       └── main.c                     # 🎯 Composition Root (Application Entry)
├── internal/                         # 🔒 PRIVATE HEADERS (not installed)
│   ├── core_internal.h
│   ├── system_internal.h
│   └── platform_internal.h

├── tests/
│   ├── unit/
│   │   ├── core/
│   │   └── system/
│   │
│   ├── integration/
│   │   └── test_end_to_end.c
│   │
│   └── fuzz/
│       └── fuzz_scanner.c
│
├── docs/
│   ├── architecture.md
│   │
│   ├── rules/
│   │   ├── 01_core_rules.md
│   │   ├── 02_system_rules.md
│   │   └── 03_platform_rules.md
│   │
│   └── governance/
│       ├── code_review.md
│       ├── contributing_guide.md
│       └── architecture_diagrams.md
│
└── build/
    └── bin/
        └── cleaner.exe




windows dialog open and shows cleaner.exe stop working. 
A probles causesd to strop working curectly. 
Windows will close the program and notify you if a solution is available 

Rules:

❌ core NEVER includes system

❌ core NEVER includes platform

❌ system NEVER includes win/posix files

❌ platform NEVER includes core

📊 Add benchmark target

🧱 Convert scanner to fully test-driven

core/      → business rules
platform/  → Windows-specific APIs
infra/     → threadpool, logging, metrics


