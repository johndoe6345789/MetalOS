# MetalOS Bootloader CMakeLists.txt
# Builds UEFI bootloader (bootx64.efi)

cmake_minimum_required(VERSION 3.16)

project(MetalOS_Bootloader C)

# Source files
set(BOOTLOADER_SOURCES
    src/main.c
)

# Header files
set(BOOTLOADER_HEADERS
    include/bootloader.h
    include/efi.h
)

# Compiler flags for UEFI
set(UEFI_CFLAGS
    -Wall
    -Wextra
    -Werror
    -ffreestanding
    -fno-stack-protector
    -fno-stack-check
    -fshort-wchar
    -mno-red-zone
    -DEFI_FUNCTION_WRAPPER
)

# Linker flags for UEFI
set(UEFI_LDFLAGS
    -shared
    -Bsymbolic
    -nostdlib
    -znocombreloc
)

# Create object library
add_library(bootloader_obj OBJECT ${BOOTLOADER_SOURCES})
target_include_directories(bootloader_obj PRIVATE include)
target_compile_options(bootloader_obj PRIVATE ${UEFI_CFLAGS})

# Create shared library (intermediate)
add_library(bootloader_so SHARED $<TARGET_OBJECTS:bootloader_obj>)
set_target_properties(bootloader_so PROPERTIES
    OUTPUT_NAME bootx64
    SUFFIX .so
    LINKER_LANGUAGE C
)
target_link_options(bootloader_so PRIVATE 
    ${UEFI_LDFLAGS}
    -T ${CMAKE_CURRENT_SOURCE_DIR}/uefi.lds
)

# Custom command to convert .so to .efi
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/bootx64.efi
    COMMAND ${CMAKE_OBJCOPY}
        -j .text -j .sdata -j .data -j .dynamic
        -j .dynsym -j .rel -j .rela -j .reloc
        --target=efi-app-x86_64
        $<TARGET_FILE:bootloader_so>
        ${CMAKE_CURRENT_BINARY_DIR}/bootx64.efi
    DEPENDS bootloader_so
    COMMENT "Converting bootloader to EFI format"
    VERBATIM
)

# Custom target for the EFI file
add_custom_target(bootloader_efi ALL
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/bootx64.efi
)

# Install target
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/bootx64.efi
    DESTINATION boot/efi/EFI/BOOT
)

# Print status
message(STATUS "Bootloader configuration:")
message(STATUS "  Sources: ${BOOTLOADER_SOURCES}")
message(STATUS "  Output: bootx64.efi")
