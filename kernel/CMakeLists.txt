# MetalOS Kernel CMakeLists.txt
# Builds minimal kernel binary

cmake_minimum_required(VERSION 3.16)

project(MetalOS_Kernel C ASM)

# Source files
# Note: Using GLOB for simplicity. For production, consider listing files explicitly.
# If new files aren't detected, re-run cmake configuration.
file(GLOB KERNEL_C_SOURCES "src/*.c")
file(GLOB KERNEL_ASM_SOURCES "src/*.asm")

set(KERNEL_SOURCES
    ${KERNEL_C_SOURCES}
)

# Find NASM assembler
find_program(NASM nasm)
if(NOT NASM)
    message(WARNING "NASM not found - assembly files will not be built")
    message(WARNING "Install NASM: sudo apt-get install nasm")
else()
    message(STATUS "Found NASM: ${NASM}")
    
    # Assemble each .asm file
    set(KERNEL_ASM_OBJECTS "")
    foreach(asm_file ${KERNEL_ASM_SOURCES})
        get_filename_component(asm_name ${asm_file} NAME_WE)
        set(asm_obj "${CMAKE_CURRENT_BINARY_DIR}/${asm_name}.o")
        
        add_custom_command(
            OUTPUT ${asm_obj}
            COMMAND ${NASM} -f elf64 ${asm_file} -o ${asm_obj}
            DEPENDS ${asm_file}
            COMMENT "Assembling ${asm_name}.asm"
            VERBATIM
        )
        
        list(APPEND KERNEL_ASM_OBJECTS ${asm_obj})
    endforeach()
endif()

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
add_executable(kernel_elf $<TARGET_OBJECTS:kernel_obj> ${KERNEL_ASM_OBJECTS})
set_target_properties(kernel_elf PROPERTIES
    OUTPUT_NAME metalos.elf
    LINKER_LANGUAGE C
    POSITION_INDEPENDENT_CODE OFF
)
target_link_options(kernel_elf PRIVATE
    -nostdlib
    -no-pie
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
