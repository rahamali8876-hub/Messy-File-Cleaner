# # tests/CMakeLists.txt

# # --------------------------------------------------
# # Platform Sources (Attach to cleaner_core)
# # --------------------------------------------------

cmake_minimum_required(VERSION 3.22)
project(cleaner VERSION 1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# ==================================================
# CORE LIBRARY (Pure + Orchestration)
# ==================================================
add_library(cleaner_core STATIC)

target_sources(cleaner_core PRIVATE
    src/core/processor.c
    src/core/error.c
    src/core/renamer.c
    src/core/core_run.c
    src/core/scanner.c   # ← ADD THIS
)

target_include_directories(cleaner_core
    PUBLIC ${PROJECT_SOURCE_DIR}/include
)

# ==================================================
# PLATFORM LIBRARY (OS Boundary)
# ==================================================
add_library(cleaner_platform STATIC)

if (WIN32)
    target_sources(cleaner_platform PRIVATE
        # platform 
        src/platform/platform_win.c
        # Thrads 
        src/platform/win/internal/threads/threads_win.c
        # Filesystem
        src/platform/win/internal/filesystem/filesystem_win.c
        # Sync
        src/platform/win/internal/sync/mutex_win.c
        src/platform/win/internal/sync/atomic_win.c
        # Threadpool
        src/platform/win/internal/threadpool/threadpool_win.c
        # Time
        src/platform/win/internal/time/time_win.c
        # Windows file backend  <-- ADD THIS
        src/platform/win/internal/windows/file_win.c
    )
endif()

target_include_directories(cleaner_platform
    PUBLIC ${PROJECT_SOURCE_DIR}/include
)

# ==================================================
# SYSTEM LIBRARY (Adapters + Infrastructure)
# ==================================================
add_library(cleaner_system STATIC)

target_sources(cleaner_system PRIVATE
    src/system/logger.c
    src/system/fs_adapter.c
    src/system/time_adapter.c
    src/system/executor.c
    src/system/metrics.c
    src/system/file_scheduler.c
)

target_include_directories(cleaner_system
    PUBLIC ${PROJECT_SOURCE_DIR}/include
)

# System depends on Platform
target_link_libraries(cleaner_system
    PUBLIC cleaner_platform
)

# Core depends on System (not reverse!)
target_link_libraries(cleaner_core
    PUBLIC cleaner_system
)

# ==================================================
# EXECUTABLE (Composition Root)
# ==================================================
add_executable(cleaner
    src/cli/main.c
)

target_link_libraries(cleaner
    PRIVATE cleaner_core
)
