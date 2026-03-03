# cmaek/CompilerWarnings.cmake
function(enable_project_warnings target)

    if (MSVC)
        target_compile_options(${target} PRIVATE
            /W4
            /WX
            /permissive-
        )
    else()
        target_compile_options(${target} PRIVATE
            -Wall
            -Wextra
            -Werror
            -Wpedantic
            -Wconversion
            -Wshadow
            -Wstrict-prototypes
            -Wmissing-prototypes
            -Wdouble-promotion
            -Wformat=2
        )
    endif()

endfunction()