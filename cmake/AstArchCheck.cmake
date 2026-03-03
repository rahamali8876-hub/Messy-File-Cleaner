# cmake/AstArchCheck.cmake


option(ENABLE_ARCH_CHECK "Enable architecture enforcement" ON)

if(NOT ENABLE_ARCH_CHECK)
    return()
endif()

find_package(Python3 COMPONENTS Interpreter REQUIRED)

# Always use the current directory of this module
set(ARCH_CHECK_SCRIPT ${CMAKE_CURRENT_LIST_DIR}/ast_layer_checker.py)

if(NOT EXISTS ${ARCH_CHECK_SCRIPT})
    message(FATAL_ERROR "ast_layer_checker.py not found in cmake/")
endif()

add_custom_target(layer_checks
    COMMAND ${Python3_EXECUTABLE} ${ARCH_CHECK_SCRIPT}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Running AST-based architecture enforcement..."
    VERBATIM
)

set(ARCH_CHECK_TARGET layer_checks PARENT_SCOPE)