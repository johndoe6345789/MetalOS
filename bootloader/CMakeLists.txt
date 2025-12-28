# MetalOS Bootloader CMakeLists

# Bootloader-specific compiler flags
set(BOOTLOADER_CFLAGS
    -fshort-wchar
    -fno-stack-check
    -DEFI_FUNCTION_WRAPPER
)

# Bootloader-specific linker flags
set(BOOTLOADER_LDFLAGS
    -shared
    -Bsymbolic
    -nostdlib
    -znocombreloc
)

# Source files
set(BOOTLOADER_SOURCES
    src/main.c
)

# Include directories
include_directories(include)

# Create bootloader object files
add_library(bootloader_objs OBJECT
    ${BOOTLOADER_SOURCES}
)

target_compile_options(bootloader_objs PRIVATE ${BOOTLOADER_CFLAGS})

# Link bootloader as shared object first
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/bootx64.so
    COMMAND ${CMAKE_LINKER}
        ${BOOTLOADER_LDFLAGS}
        -T ${CMAKE_CURRENT_SOURCE_DIR}/uefi.lds
        $<TARGET_OBJECTS:bootloader_objs>
        -o ${CMAKE_CURRENT_BINARY_DIR}/bootx64.so
    DEPENDS bootloader_objs ${CMAKE_CURRENT_SOURCE_DIR}/uefi.lds
    COMMENT "Linking bootloader shared object"
    VERBATIM
)

# Convert to EFI binary
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/bootx64.efi
    COMMAND ${CMAKE_OBJCOPY}
        -j .text -j .sdata -j .data -j .dynamic
        -j .dynsym -j .rel -j .rela -j .reloc
        --target=efi-app-x86_64
        ${CMAKE_CURRENT_BINARY_DIR}/bootx64.so
        ${CMAKE_CURRENT_BINARY_DIR}/bootx64.efi
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/bootx64.so
    COMMENT "Creating EFI bootloader binary"
    VERBATIM
)

add_custom_target(bootloader ALL
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/bootx64.efi
)

# Install bootloader binary
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/bootx64.efi
    DESTINATION bin
)
