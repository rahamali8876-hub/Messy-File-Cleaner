# cmake/LayerEnforcement.cmake

function(add_architecture_checks)

    find_package(Python3 REQUIRED)

    add_custom_target(layer_checks
        COMMAND ${Python3_EXECUTABLE}
            ${CMAKE_SOURCE_DIR}/tools/ast_arch_checker.py
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Running AST-based architectural enforcement..."
    )

endfunction()