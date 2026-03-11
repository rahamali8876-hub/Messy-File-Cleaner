# Tests subdirectory

# Find testing framework if available
find_package(Check QUIET)

if(NOT Check_FOUND)
    message(STATUS "Check framework not found. Skipping unit tests.")
    return()
endif()

# Collect test sources
file(GLOB TEST_SOURCES CONFIGURE_DEPENDS
    test_*.c
)

# Create test executable for each test file
foreach(TEST_SRC ${TEST_SOURCES})
    get_filename_component(TEST_NAME ${TEST_SRC} NAME_WE)
    add_executable(${TEST_NAME} ${TEST_SRC})
    target_link_libraries(${TEST_NAME} PRIVATE cleaner_core Check::Check)
    target_include_directories(${TEST_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)
    
    # Enable project warnings
    enable_project_warnings(${TEST_NAME})
    
    # Register test
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endforeach()
