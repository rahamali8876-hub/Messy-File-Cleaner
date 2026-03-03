# cmake/ Sanitizers.cmake

function(enable_sanitizers target)

    if(NOT ENABLE_SANITIZERS)
        return()
    endif()

    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${target} PRIVATE
            -fsanitize=address
            -fsanitize=undefined
            -fno-omit-frame-pointer
        )

        target_link_options(${target} PRIVATE
            -fsanitize=address
            -fsanitize=undefined
        )
    endif()

endfunction()