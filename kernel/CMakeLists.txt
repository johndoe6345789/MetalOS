# MetalOS Kernel CMakeLists

# Include directories
include_directories(include)

# Source files
set(KERNEL_C_SOURCES
    src/main.c
    src/gdt.c
    src/interrupts.c
    src/memory.c
    src/pci.c
    src/timer.c
)

set(KERNEL_ASM_SOURCES
    src/gdt_flush.asm
    src/interrupts_asm.asm
)

# Kernel-specific C compiler flags
set(KERNEL_CFLAGS
    -mcmodel=large
    -O2
)

# Create kernel C object files
add_library(kernel_c_objs OBJECT
    ${KERNEL_C_SOURCES}
)

target_compile_options(kernel_c_objs PRIVATE ${KERNEL_CFLAGS})
target_include_directories(kernel_c_objs PRIVATE include)

# Create kernel ASM object files separately
# We need to avoid CMake adding C flags to NASM
foreach(asm_file ${KERNEL_ASM_SOURCES})
    get_filename_component(asm_name ${asm_file} NAME_WE)
    set(asm_obj ${CMAKE_CURRENT_BINARY_DIR}/${asm_name}.o)
    
    add_custom_command(
        OUTPUT ${asm_obj}
        COMMAND ${CMAKE_ASM_NASM_COMPILER}
            -f elf64
            -I ${CMAKE_CURRENT_SOURCE_DIR}/include
            -o ${asm_obj}
            ${CMAKE_CURRENT_SOURCE_DIR}/${asm_file}
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${asm_file}
        COMMENT "Assembling ${asm_file}"
        VERBATIM
    )
    
    list(APPEND KERNEL_ASM_OBJS ${asm_obj})
endforeach()

# Custom target for ASM objects
add_custom_target(kernel_asm_objs
    DEPENDS ${KERNEL_ASM_OBJS}
)

# Link kernel binary
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/metalos.bin
    COMMAND ${CMAKE_LINKER}
        -nostdlib
        -T ${CMAKE_CURRENT_SOURCE_DIR}/linker.ld
        $<TARGET_OBJECTS:kernel_c_objs>
        ${KERNEL_ASM_OBJS}
        -o ${CMAKE_CURRENT_BINARY_DIR}/metalos.bin
    DEPENDS kernel_c_objs kernel_asm_objs ${CMAKE_CURRENT_SOURCE_DIR}/linker.ld
    COMMENT "Linking kernel binary"
    COMMAND_EXPAND_LISTS
)

add_custom_target(kernel ALL
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/metalos.bin
)

# Install kernel binary
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/metalos.bin
    DESTINATION bin
)
