### inatallation packeges ( on MINGW TERMINAL )

        pacman -S mingw-w64-x86_64-libjpeg-turbo
        pacman -S mingw-w64-x86_64-make
        pacman -S mingw-w64-x86_64-cppcheck
        pacman -S libjpeg-turbo
        pacman -S make

### REMOVE PACKEGES

        pacman -R mingw-w64-x86_64-make

### filter installed packeges

        pacman -Q
        pacman -Q | grep make
        pacman -Q | grep jpeg
        mingw-w64-x86_64-make 4.4.1-4
        mingw-w64-x86_64-libjpeg-turbo 3.1.3-1

### Find DLL Inside FILES

        nm -g build/img2pdf.dll | grep img2pdf_

### NORMAL MODE How To Build ( Windows MinGW )

        rmdir /s /q build
        cmake -S . -B build -G "MinGW Makefiles"
        cmake --build build
        cd build
        cleaner.exe --source "D:\Testing" --dry-run

### DEV MODE

        cmake -S . -B build -G "MinGW Makefiles" -DDEV_MODE=ON
        cmake --build build

### DEBUG ( GDB )

        gdb cleaner.exe
        (gdb) run --source "D:\Testing" --dry-run

### Testing VIOLETION ( FORMATING / STATIC ANALYSIS / LAYER CHECKS / ETC. )

        cmake --build build --target help

        cmake --build build --target layer_checks
        cmake --build build --target format
        cmake --build build --target format_check
        cmake --build build --target static_analysis ( After building only )
        cmake --build build --target cppcheck
        cmake --build build --target docs
        cmake --build build --target coverage
        cmake --build build --target graph
        cmake --build build --target all_quality
        cmake --build build --target distclean
        

### Remove build artifacts

        rmdir /S /Q build

### 🔥 Debug Build

        cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug ..

### 🔥 Release Build

        cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release ..
        cmake --build .

# ==========================================================

# Enterprise-Grade Cleaner - CMakeLists.txt

# Windows + MinGW friendly

# ==========================================================

cmake_minimum_required(VERSION 3.25)

project(cleaner
    VERSION 1.0.0
    DESCRIPTION "Enterprise-grade layered C system"
    LANGUAGES C
)

# ==========================================================

# Developer Mode (Professional Dev Environment)

# ==========================================================

option(DEV_MODE "Enable developer tools" OFF)

if(DEV_MODE)

    message(STATUS "Developer Mode Enabled")

    if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE Debug CACHE STRING "" FORCE)
    endif()

    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")

        add_compile_options(
            -Wall
            -Wextra
            -Wpedantic
            -Wshadow
            -Wconversion
            -Wnull-dereference
            -Wdouble-promotion
            -Wformat=2
        )

        add_compile_options(
            -fsanitize=address
            -fsanitize=undefined
            -fno-omit-frame-pointer
            -g
        )

        add_link_options(
            -fsanitize=address
            -fsanitize=undefined
        )

    endif()

endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ==========================================================

# Global Policies

# ==========================================================

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR "In-source builds are forbidden. Please use a separate build directory.")
endif()

# ==========================================================

# Options

# ==========================================================

option(ENABLE_TESTING "Enable tests" ON)
option(ENABLE_STATIC_ANALYSIS "Enable static analysis" ON)

# ==========================================================

# Include custom CMake modules

# ==========================================================

include(cmake/CompilerWarnings.cmake)
include(cmake/Sanitizers.cmake OPTIONAL)

# adding senetizer

set(CMAKE_C_FLAGS_DEBUG "-O0 -g3 -fsanitize=address -fsanitize=undefined")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-fsanitize=address -g)
    add_link_options(-fsanitize=address)
endif()

# ==========================================================

# Source files

# ==========================================================

file(GLOB_RECURSE CORE_SOURCES CONFIGURE_DEPENDS src/core/*.c)
file(GLOB_RECURSE WAL_SOURCES CONFIGURE_DEPENDS src/wal/*.c)
file(GLOB_RECURSE SYSTEM_SOURCES CONFIGURE_DEPENDS src/system/*.c)
file(GLOB_RECURSE PLATFORM_SOURCES CONFIGURE_DEPENDS src/platform/*.c)

set(CLEANER_CORE_SOURCES
    ${CORE_SOURCES}
    ${WAL_SOURCES}
    ${SYSTEM_SOURCES}
    ${PLATFORM_SOURCES}
)

# ==========================================================

# Static library: cleaner_core

# ==========================================================

add_library(cleaner_core STATIC ${CLEANER_CORE_SOURCES})

target_include_directories(cleaner_core
PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

enable_project_warnings(cleaner_core)
enable_sanitizers(cleaner_core)

# ==========================================================

# Filesystem Plugin (DLL)

# ==========================================================

add_library(cleaner_fs_windows SHARED
    plugins/fs_windows.c
)

target_include_directories(cleaner_fs_windows
PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

set_target_properties(cleaner_fs_windows PROPERTIES
    OUTPUT_NAME "cleaner_fs_windows"
)

# ==========================================================

# Executable

# ==========================================================

add_executable(cleaner
    src/app/main.c
)

target_link_libraries(cleaner
PRIVATE
    cleaner_core
)

# ==========================================================

# Testing

# ==========================================================

if(ENABLE_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()

# ==========================================================

# Formatting / Static Analysis

# ==========================================================

find_program(CLANG_FORMAT_EXE NAMES clang-format)

if(CLANG_FORMAT_EXE)
    file(GLOB_RECURSE ALL_C_FILES CONFIGURE_DEPENDS src/*.c include/*.h)

    add_custom_target(format
        COMMAND ${CLANG_FORMAT_EXE} -i ${ALL_C_FILES}
        COMMENT "Formatting source code..."
    )

    add_custom_target(format_check
        COMMAND ${CLANG_FORMAT_EXE} --dry-run --Werror ${ALL_C_FILES}
        COMMENT "Checking formatting..."
    )
endif()

find_program(CLANG_TIDY_EXE NAMES clang-tidy)

if(CLANG_TIDY_EXE)
    add_custom_target(static_analysis
        COMMAND ${CLANG_TIDY_EXE} -p ${CMAKE_BINARY_DIR} ${ALL_C_FILES}
        COMMENT "Running clang-tidy..."
    )
endif()

find_program(CPPCHECK_EXE NAMES cppcheck)

if(CPPCHECK_EXE)
    add_custom_target(cppcheck
        COMMAND ${CPPCHECK_EXE}
        --enable=all
        --inconclusive
        --error-exitcode=1
        ${CMAKE_SOURCE_DIR}/src
        COMMENT "Running cppcheck..."
    )
endif()

# ==========================================================

# Install

# ==========================================================

install(TARGETS cleaner cleaner_core cleaner_fs_windows
    RUNTIME DESTINATION bin
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)

install(DIRECTORY include/
    DESTINATION include
)
