# MetalOS Root CMakeLists.txt
cmake_minimum_required(VERSION 3.16)

project(MetalOS 
    VERSION 0.1.0
    DESCRIPTION "Minimal OS for QT6 applications"
    LANGUAGES C CXX ASM
)

# Set C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Export compile commands for IDE support
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Compiler detection and configuration
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    message(STATUS "Using Clang compiler")
    set(USING_CLANG TRUE)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    message(STATUS "Using GCC compiler")
    set(USING_GCC TRUE)
else()
    message(WARNING "Unsupported compiler: ${CMAKE_CXX_COMPILER_ID}")
endif()

# Default to Release build if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Options
option(BUILD_BOOTLOADER "Build UEFI bootloader" ON)
option(BUILD_KERNEL "Build kernel" ON)
option(BUILD_TESTS "Build unit tests" ON)

# Build output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Create build directory for image
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/build)

# Add subdirectories
if(BUILD_BOOTLOADER)
    add_subdirectory(bootloader)
endif()

if(BUILD_KERNEL)
    add_subdirectory(kernel)
endif()

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Custom target to create bootable image
find_program(MFORMAT mformat)
find_program(MCOPY mcopy)
find_program(MMD mmd)

if(MFORMAT AND MCOPY AND MMD)
    add_custom_target(image
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/build/iso/EFI/BOOT
        COMMAND ${CMAKE_COMMAND} -E copy 
            ${CMAKE_BINARY_DIR}/bootloader/bootx64.efi 
            ${CMAKE_BINARY_DIR}/build/iso/EFI/BOOT/
        COMMAND ${CMAKE_COMMAND} -E copy 
            ${CMAKE_BINARY_DIR}/kernel/metalos.bin 
            ${CMAKE_BINARY_DIR}/build/iso/
        COMMAND ${CMAKE_COMMAND} -E echo "Creating disk image..."
        COMMAND dd if=/dev/zero of=${CMAKE_BINARY_DIR}/build/metalos.img bs=1M count=64
        COMMAND ${MFORMAT} -i ${CMAKE_BINARY_DIR}/build/metalos.img -F -v METALOS ::
        COMMAND ${MMD} -i ${CMAKE_BINARY_DIR}/build/metalos.img ::/EFI
        COMMAND ${MMD} -i ${CMAKE_BINARY_DIR}/build/metalos.img ::/EFI/BOOT
        COMMAND ${MCOPY} -i ${CMAKE_BINARY_DIR}/build/metalos.img 
            ${CMAKE_BINARY_DIR}/build/iso/EFI/BOOT/bootx64.efi ::/EFI/BOOT/
        COMMAND ${MCOPY} -i ${CMAKE_BINARY_DIR}/build/metalos.img 
            ${CMAKE_BINARY_DIR}/build/iso/metalos.bin ::/
        COMMAND ${CMAKE_COMMAND} -E echo "Success! Created ${CMAKE_BINARY_DIR}/build/metalos.img"
        DEPENDS bootloader_efi kernel_bin
        COMMENT "Creating bootable disk image"
        VERBATIM
    )
else()
    message(WARNING "mtools not found - 'image' target will not be available")
endif()

# Custom target to run in QEMU
find_program(QEMU qemu-system-x86_64)
if(QEMU)
    # Find OVMF firmware
    set(OVMF_PATHS
        /usr/share/OVMF/OVMF_CODE.fd
        /usr/share/ovmf/OVMF.fd
        /usr/share/edk2-ovmf/x64/OVMF_CODE.fd
        /usr/share/qemu/ovmf-x86_64.bin
    )
    
    foreach(ovmf_path ${OVMF_PATHS})
        if(EXISTS ${ovmf_path})
            set(OVMF_FIRMWARE ${ovmf_path})
            break()
        endif()
    endforeach()
    
    if(OVMF_FIRMWARE)
        message(STATUS "Found OVMF firmware: ${OVMF_FIRMWARE}")
        
        add_custom_target(qemu
            COMMAND ${QEMU}
                -drive if=pflash,format=raw,readonly=on,file=${OVMF_FIRMWARE}
                -drive format=raw,file=${CMAKE_BINARY_DIR}/build/metalos.img
                -m 512M
                -serial stdio
                -display none
                -net none
            DEPENDS image
            COMMENT "Running MetalOS in QEMU"
            VERBATIM
        )
        
        add_custom_target(qemu-debug
            COMMAND ${QEMU}
                -drive if=pflash,format=raw,readonly=on,file=${OVMF_FIRMWARE}
                -drive format=raw,file=${CMAKE_BINARY_DIR}/build/metalos.img
                -m 512M
                -serial stdio
                -display none
                -net none
                -d int,cpu_reset
            DEPENDS image
            COMMENT "Running MetalOS in QEMU with debug output"
            VERBATIM
        )
        
        add_custom_target(qemu-gdb
            COMMAND ${QEMU}
                -drive if=pflash,format=raw,readonly=on,file=${OVMF_FIRMWARE}
                -drive format=raw,file=${CMAKE_BINARY_DIR}/build/metalos.img
                -m 512M
                -serial stdio
                -display none
                -net none
                -s -S
            DEPENDS image
            COMMENT "Running MetalOS in QEMU with GDB server (port 1234)"
            VERBATIM
        )
        
        add_custom_target(qemu-uefi-test
            COMMAND ${CMAKE_COMMAND} -E echo "Testing QEMU UEFI boot (no OS image)..."
            COMMAND ${CMAKE_COMMAND} -E echo "Using OVMF firmware: ${OVMF_FIRMWARE}"
            COMMAND ${CMAKE_COMMAND} -E echo "This will boot to UEFI shell in nographic mode."
            COMMAND ${CMAKE_COMMAND} -E echo "You should see UEFI boot messages. Press Ctrl-A then X to exit QEMU."
            COMMAND ${QEMU}
                -drive if=pflash,format=raw,readonly=on,file=${OVMF_FIRMWARE}
                -m 512M
                -nographic
                -net none
            COMMENT "Testing QEMU UEFI setup without OS image"
            VERBATIM
        )
    else()
        message(WARNING "OVMF firmware not found - QEMU targets will not be available")
    endif()
else()
    message(WARNING "QEMU not found - QEMU targets will not be available")
endif()

# Print configuration summary
message(STATUS "")
message(STATUS "MetalOS Configuration:")
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  Bootloader: ${BUILD_BOOTLOADER}")
message(STATUS "  Kernel: ${BUILD_KERNEL}")
message(STATUS "  Tests: ${BUILD_TESTS}")
message(STATUS "")
