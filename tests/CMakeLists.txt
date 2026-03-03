# # ==================================================
# # Enterprise-Grade Enforcement Notes
# # ==================================================
# # ✅ Core layer: pure, deterministic, no OS/Platform/System dependencies
# # ✅ System layer: orchestrates Core, uses Platform abstractions only
# # ✅ Platform layer: wraps OS, no business logic
# # ✅ Executable: links only System, which links Core+Platform
# # ✅ Tests separated: unit / integration / benchmarks
# # ✅ Optional: AddressSanitizer, static analysis

# cmake_minimum_required(VERSION 3.25)

# project(cleaner
#     VERSION 1.0.0
#     DESCRIPTION "Enterprise-grade layered C system"
#     LANGUAGES C
# )

# # ========================
# # Global Policies
# # ========================
# set(CMAKE_C_STANDARD 17)
# set(CMAKE_C_STANDARD_REQUIRED ON)
# set(CMAKE_C_EXTENSIONS OFF)

# if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
#     message(FATAL_ERROR "In-source builds are forbidden.")
# endif()

# # ========================
# # Options
# # ========================
# option(ENABLE_TESTING "Enable tests" ON)
# option(ENABLE_SANITIZERS "Enable sanitizers" ON)
# option(ENABLE_STATIC_ANALYSIS "Enable static analysis" ON)
# option(ENABLE_ARCH_CHECK "Enable architecture enforcement" ON)

# include(cmake/CompilerWarnings.cmake)
# include(cmake/Sanitizers.cmake)
# include(cmake/LayerEnforcement.cmake)

# # ========================
# # Collect Sources
# # ========================
# file(GLOB_RECURSE CLEANER_SOURCES CONFIGURE_DEPENDS
#     src/*.c
# )

# file(GLOB_RECURSE ALL_C_FILES CONFIGURE_DEPENDS
#     src/*.c
#     include/*.h
# )

# # ========================
# # Core Library
# # ========================
# add_library(cleaner_core STATIC ${CLEANER_SOURCES})

# target_include_directories(cleaner_core
#     PUBLIC
#         $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
# )

# enable_project_warnings(cleaner_core)
# enable_sanitizers(cleaner_core)

# # ========================
# # CLI
# # ========================
# add_executable(cleaner
#     src/app/main.c
# )

# target_link_libraries(cleaner PRIVATE cleaner_core)

# # ========================
# # Tests
# # ========================
# if(ENABLE_TESTING)
#     enable_testing()
#     add_subdirectory(tests)
# endif()

# # ========================
# # 1️⃣ Architecture Enforcement (Python AST Checker)
# # ========================
# if(ENABLE_ARCH_CHECK)
#     find_package(Python3 COMPONENTS Interpreter REQUIRED)

#     set(AST_CHECK_SCRIPT "${CMAKE_SOURCE_DIR}/cmake/ast_layer_checker.py")

#     if(NOT EXISTS ${AST_CHECK_SCRIPT})
#         message(FATAL_ERROR "cmake/ast_layer_checker.py not found!")
#     endif()

#     add_custom_target(layer_checks
#         COMMAND ${Python3_EXECUTABLE} ${AST_CHECK_SCRIPT} ${CMAKE_SOURCE_DIR}
#         WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
#         COMMENT "Running AST-based architecture enforcement..."
#         VERBATIM
#     )

#     add_dependencies(cleaner_core layer_checks)
# endif()

# # ========================
# # 2️⃣ Format (clang-format)
# # ========================
# find_program(CLANG_FORMAT_EXE NAMES clang-format)

# if(CLANG_FORMAT_EXE)
#     add_custom_target(format
#         COMMAND ${CLANG_FORMAT_EXE} -i ${ALL_C_FILES}
#         COMMENT "Formatting source code..."
#     )

#     add_custom_target(format_check
#         COMMAND ${CLANG_FORMAT_EXE} --dry-run --Werror ${ALL_C_FILES}
#         COMMENT "Checking formatting..."
#     )
# endif()

# # ========================
# # 3️⃣ clang-tidy Static Analysis
# # ========================
# find_program(CLANG_TIDY_EXE NAMES clang-tidy)

# if(CLANG_TIDY_EXE)
#     add_custom_target(static_analysis
#         COMMAND ${CLANG_TIDY_EXE} -p ${CMAKE_BINARY_DIR} ${ALL_C_FILES}
#         COMMENT "Running clang-tidy..."
#     )
# endif()

# # ========================
# # 4️⃣ cppcheck
# # ========================
# find_program(CPPCHECK_EXE NAMES cppcheck)

# if(CPPCHECK_EXE)
#     add_custom_target(cppcheck
#         COMMAND ${CPPCHECK_EXE} --enable=all --inconclusive --error-exitcode=1 ${CMAKE_SOURCE_DIR}/src
#         COMMENT "Running cppcheck..."
#     )
# endif()

# # ========================
# # 5️⃣ Documentation (Doxygen)
# # ========================
# find_package(Doxygen)

# if(DOXYGEN_FOUND)
#     add_custom_target(docs
#         COMMAND ${DOXYGEN_EXECUTABLE} Doxyfile
#         WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
#         COMMENT "Generating documentation..."
#     )
# endif()

# # ========================
# # 6️⃣ Coverage (GCC/Clang)
# # ========================
# if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
#     add_custom_target(coverage
#         COMMAND gcovr -r ${CMAKE_SOURCE_DIR}
#         COMMENT "Generating coverage report..."
#     )
# endif()

# # ========================
# # 7️⃣ Build Graph (CMake Graphviz)
# # ========================
# add_custom_target(graph
#     COMMAND ${CMAKE_COMMAND} --graphviz=${CMAKE_BINARY_DIR}/build_graph.dot ${CMAKE_SOURCE_DIR}
#     COMMENT "Generating CMake dependency graph..."
# )

# # ========================
# # 8️⃣ Clean Build Artifacts
# # ========================
# add_custom_target(distclean
#     COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_BINARY_DIR}
#     COMMENT "Removing build directory..."
# )

# # ========================
# # 9️⃣ Full Quality Gate
# # ========================
# add_custom_target(all_quality
#     DEPENDS
#         layer_checks
#         format_check
#         static_analysis
#         cppcheck
# )

# # ========================
# # 🔟 Install
# # ========================
# install(TARGETS cleaner cleaner_core
#     RUNTIME DESTINATION bin
#     ARCHIVE DESTINATION lib
# )

# install(DIRECTORY include/
#     DESTINATION include
# )