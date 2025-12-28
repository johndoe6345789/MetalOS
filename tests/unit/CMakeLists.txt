# MetalOS Unit Tests CMakeLists

# Test sources
set(TEST_SOURCES
    test_bootloader.c
)

# Create test executable for each test file
foreach(test_src ${TEST_SOURCES})
    get_filename_component(test_name ${test_src} NAME_WE)
    
    add_executable(${test_name} ${test_src})
    
    # Link with test framework (if needed)
    target_include_directories(${test_name} PRIVATE
        ${CMAKE_SOURCE_DIR}/tests/include
        ${CMAKE_SOURCE_DIR}/bootloader/include
        ${CMAKE_SOURCE_DIR}/kernel/include
    )
    
    # Add as test
    add_test(NAME ${test_name} COMMAND ${test_name})
endforeach()
