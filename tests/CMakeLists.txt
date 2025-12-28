# MetalOS Tests CMakeLists.txt
# Builds and runs unit tests

cmake_minimum_required(VERSION 3.16)

project(MetalOS_Tests C)

# Find all test source files
file(GLOB TEST_SOURCES "unit/*.c")

# Include directories
include_directories(
    include
    ../bootloader/include
    ../kernel/include
)

# Create a test executable for each test file
foreach(test_source ${TEST_SOURCES})
    # Get the test name from the file name
    get_filename_component(test_name ${test_source} NAME_WE)
    
    # Create test executable
    add_executable(${test_name} ${test_source})
    
    # Set compiler flags
    target_compile_options(${test_name} PRIVATE
        -Wall
        -Wextra
        -std=c11
    )
    
    # Add as a test
    add_test(NAME ${test_name} COMMAND ${test_name})
endforeach()

# Custom target to run all tests
add_custom_target(run_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    DEPENDS ${TEST_SOURCES}
    COMMENT "Running all tests"
    VERBATIM
)

# Print status
message(STATUS "Tests configuration:")
message(STATUS "  Test sources: ${TEST_SOURCES}")
