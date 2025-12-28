# MetalOS Kernel CMakeLists.txt
# Builds minimal kernel binary

cmake_minimum_required(VERSION 3.16)

project(MetalOS_Kernel C ASM)

# Source files
set(KERNEL_SOURCES
    src/main.c
)

# Find all source files in subdirectories
file(GLOB_RECURSE CORE_SOURCES "src/core/*.c")
file(GLOB_RECURSE HAL_SOURCES "src/hal/*.c")
file(GLOB_RECURSE DRIVER_SOURCES "src/drivers/*.c")
file(GLOB_RECURSE SYSCALL_SOURCES "src/syscall/*.c")

list(APPEND KERNEL_SOURCES 
    ${CORE_SOURCES}
    ${HAL_SOURCES}
    ${DRIVER_SOURCES}
    ${SYSCALL_SOURCES}
)

# Compiler flags for kernel
set(KERNEL_CFLAGS
    -Wall
    -Wextra
    -Werror
    -ffreestanding
    -fno-stack-protector
    -mno-red-zone
    -mcmodel=large
    -O2
)

# Create object library
add_library(kernel_obj OBJECT ${KERNEL_SOURCES})
target_include_directories(kernel_obj PRIVATE include)
target_compile_options(kernel_obj PRIVATE ${KERNEL_CFLAGS})

# Create kernel binary
add_executable(kernel_elf $<TARGET_OBJECTS:kernel_obj>)
set_target_properties(kernel_elf PROPERTIES
    OUTPUT_NAME metalos.elf
    LINKER_LANGUAGE C
)
target_link_options(kernel_elf PRIVATE
    -nostdlib
    -T ${CMAKE_CURRENT_SOURCE_DIR}/linker.ld
)

# Custom command to create flat binary
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/metalos.bin
    COMMAND ${CMAKE_OBJCOPY}
        -O binary
        $<TARGET_FILE:kernel_elf>
        ${CMAKE_CURRENT_BINARY_DIR}/metalos.bin
    DEPENDS kernel_elf
    COMMENT "Creating kernel flat binary"
    VERBATIM
)

# Custom target for the binary
add_custom_target(kernel_bin ALL
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/metalos.bin
)

# Install target
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/metalos.bin
    DESTINATION boot
)

# Print status
message(STATUS "Kernel configuration:")
message(STATUS "  Sources: ${KERNEL_SOURCES}")
message(STATUS "  Output: metalos.bin")
