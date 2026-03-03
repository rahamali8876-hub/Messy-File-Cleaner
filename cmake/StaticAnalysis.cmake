# cmake/StaticAnalysis.cmake

if(ENABLE_STATIC_ANALYSIS)

    find_program(CLANG_TIDY_EXE NAMES clang-tidy)

    if(CLANG_TIDY_EXE)
        set(CMAKE_C_CLANG_TIDY "${CLANG_TIDY_EXE}")
    endif()

endif()