cmake_minimum_required(VERSION 3.25)

project(cleaner
    VERSION 1.0.0
    DESCRIPTION "Enterprise-grade layered C system"
    LANGUAGES C
)

# ========================
# Global Policies
# ========================
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR "In-source builds are forbidden.")
endif()

# ========================
# Options
# ========================
option(ENABLE_TESTING "Enable tests" ON)
option(ENABLE_SANITIZERS "Enable sanitizers" OFF)
option(ENABLE_STATIC_ANALYSIS "Enable static analysis" ON)

# ========================
# Include custom CMake modules
# ========================
include(cmake/CompilerWarnings.cmake)

# ========================
# Source Collection
# ========================
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

file(GLOB_RECURSE ALL_C_FILES CONFIGURE_DEPENDS src/*.c include/*.h)

# ========================
# Static Library: cleaner_core
# ========================
add_library(cleaner_core STATIC ${CLEANER_CORE_SOURCES})

target_include_directories(cleaner_core
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
)

enable_project_warnings(cleaner_core)

# ========================
# Executable: cleaner
# ========================
add_executable(cleaner src/app/main.c)
target_link_libraries(cleaner PRIVATE cleaner_core)

# ========================
# Testing
# ========================
if(ENABLE_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()

# ========================
# Code Formatting
# ========================
find_program(CLANG_FORMAT_EXE NAMES clang-format)
if(CLANG_FORMAT_EXE)
    add_custom_target(format
        COMMAND ${CLANG_FORMAT_EXE} -i ${ALL_C_FILES}
        COMMENT "Formatting source code..."
    )
    add_custom_target(format_check
        COMMAND ${CLANG_FORMAT_EXE} --dry-run --Werror ${ALL_C_FILES}
        COMMENT "Checking formatting..."
    )
endif()

# ========================
# Static Analysis
# ========================
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
        COMMAND ${CPPCHECK_EXE} --enable=all --inconclusive --error-exitcode=1 ${CMAKE_SOURCE_DIR}/src
        COMMENT "Running cppcheck..."
    )
endif()

# ========================
# Benchmarks
# ========================
# file(GLOB BENCHMARK_SOURCES CONFIGURE_DEPENDS benchmarks/bench_*.c)
# foreach(BM_SRC ${BENCHMARK_SOURCES})
#     get_filename_component(BM_NAME ${BM_SRC} NAME_WE)
#     add_executable(${BM_NAME} ${BM_SRC})
#     target_link_libraries(${BM_NAME} PRIVATE cleaner_core)
#     enable_project_warnings(${BM_NAME})
# endforeach()

# ========================
# Install
# ========================
install(TARGETS cleaner cleaner_core
    RUNTIME DESTINATION bin
    ARCHIVE DESTINATION lib
)

install(DIRECTORY include/
    DESTINATION include
)
