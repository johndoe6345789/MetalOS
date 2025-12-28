cmake_minimum_required(VERSION 3.16)

project(MetalOS 
    VERSION 0.1.0
    DESCRIPTION "Minimal OS for QT6 on AMD64 + RX 6600"
    LANGUAGES C CXX ASM_NASM
)

# Enable assembly with NASM
enable_language(ASM_NASM)

# Set C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Set C++ standard (for QT6 app later)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Platform-specific flags
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

# Global compile options for bare metal
add_compile_options(
    -Wall
    -Wextra
    -Werror
    -ffreestanding
    -fno-stack-protector
    -mno-red-zone
)

# NASM flags for assembly files
set(CMAKE_ASM_NASM_OBJECT_FORMAT elf64)
set(CMAKE_ASM_NASM_COMPILE_OBJECT "<CMAKE_ASM_NASM_COMPILER> <INCLUDES> <FLAGS> -f ${CMAKE_ASM_NASM_OBJECT_FORMAT} -o <OBJECT> <SOURCE>")

# Add subdirectories
add_subdirectory(bootloader)
add_subdirectory(kernel)
add_subdirectory(userspace)

# Testing
enable_testing()
add_subdirectory(tests)

# Custom target to create bootable image
add_custom_target(image
    COMMAND ${CMAKE_SOURCE_DIR}/scripts/create_image.sh
    DEPENDS bootloader kernel
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Creating bootable disk image"
)

# QEMU targets for testing
set(QEMU_DISPLAY "none" CACHE STRING "QEMU display mode (none, gtk, sdl)")

# Find OVMF firmware
find_file(OVMF_FIRMWARE
    NAMES OVMF_CODE.fd OVMF.fd ovmf-x86_64.bin
    PATHS
        /usr/share/OVMF
        /usr/share/ovmf
        /usr/share/edk2-ovmf/x64
        /usr/share/qemu
    DOC "UEFI firmware for QEMU"
)

if(OVMF_FIRMWARE)
    message(STATUS "Found OVMF firmware: ${OVMF_FIRMWARE}")
    
    # QEMU run target
    add_custom_target(qemu
        COMMAND qemu-system-x86_64
            -drive if=pflash,format=raw,readonly=on,file=${OVMF_FIRMWARE}
            -drive format=raw,file=${CMAKE_BINARY_DIR}/metalos.img
            -m 512M
            -serial stdio
            -display ${QEMU_DISPLAY}
            -net none
        DEPENDS image
        COMMENT "Running MetalOS in QEMU with UEFI"
    )
    
    # QEMU debug target
    add_custom_target(qemu-debug
        COMMAND qemu-system-x86_64
            -drive if=pflash,format=raw,readonly=on,file=${OVMF_FIRMWARE}
            -drive format=raw,file=${CMAKE_BINARY_DIR}/metalos.img
            -m 512M
            -serial stdio
            -display ${QEMU_DISPLAY}
            -net none
            -d int,cpu_reset
        DEPENDS image
        COMMENT "Running MetalOS in QEMU with debug output"
    )
    
    # QEMU GDB target
    add_custom_target(qemu-gdb
        COMMAND qemu-system-x86_64
            -drive if=pflash,format=raw,readonly=on,file=${OVMF_FIRMWARE}
            -drive format=raw,file=${CMAKE_BINARY_DIR}/metalos.img
            -m 512M
            -serial stdio
            -display ${QEMU_DISPLAY}
            -net none
            -s -S
        DEPENDS image
        COMMENT "Running MetalOS in QEMU with GDB server on port 1234"
    )
    
    # QEMU UEFI test (no OS image)
    add_custom_target(qemu-uefi-test
        COMMAND qemu-system-x86_64
            -drive if=pflash,format=raw,readonly=on,file=${OVMF_FIRMWARE}
            -m 512M
            -nographic
            -net none
        COMMENT "Testing QEMU UEFI boot (no OS image)"
    )
else()
    message(WARNING "OVMF firmware not found. QEMU targets will not be available.")
    message(WARNING "Install OVMF:")
    message(WARNING "  Ubuntu/Debian: sudo apt-get install ovmf")
    message(WARNING "  Arch Linux: sudo pacman -S edk2-ovmf")
    message(WARNING "  Fedora: sudo dnf install edk2-ovmf")
endif()

# Print build configuration
message(STATUS "MetalOS Build Configuration:")
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  C Compiler: ${CMAKE_C_COMPILER}")
message(STATUS "  C++ Compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "  NASM: ${CMAKE_ASM_NASM_COMPILER}")
message(STATUS "  QEMU Display: ${QEMU_DISPLAY}")
