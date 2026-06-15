cmake_minimum_required(VERSION 4.2)

# Define target architecture (default to x86_64)
set(ARCH "x86_64" CACHE STRING "Target architecture to build for")

# Force a freestanding, bare-metal environment configuration
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR ${ARCH})

# Tell CMake not to look for standard system libraries or compiler runtimes
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
project(BareMetalKernel C CXX ASM)

# Define the Clang cross-compilation flags
# Generic freestanding flags for all architectures
set(COMMON_FLAGS
        "-ffreestanding"
        "-fno-exceptions"
        "-fno-rtti"
        "-fno-pic"
        "-fno-pie"
        "-O3"
        "-fomit-frame-pointer"
)

# Architecture-specific flags and sources
if(ARCH STREQUAL "x86_64")
    list(APPEND ARCH_FLAGS
        "-target" "x86_64-unknown-none-elf"
        "-mcmodel=kernel"
        "-mno-red-zone"
        "-mno-sse"
        "-mno-sse2"
        "-mno-mmx"
        "-mno-80387"
        "-mgeneral-regs-only"
    )

    list(APPEND ARCH_LINK_FLAGS "-m" "elf_x86_64")

    # x86_64 and PC specific sources
    list(APPEND ARCH_SRCS
            src/arch/x86_64/gdt.cpp
            src/arch/x86_64/idt.cpp
            src/arch/x86_64/interrupts.cpp
            src/arch/x86_64/interrupts.S
            src/drivers/pic.cpp
            src/drivers/keyboard.cpp
            src/drivers/mouse.cpp
            src/arch/x86_64/cpu.cpp
    )

    list(APPEND ARCH_HEADERS
            include/arch/x86_64/gdt.h
            include/arch/x86_64/idt.h
            include/arch/x86_64/io.h
            include/arch/x86_64/interrupts.h
    )
elseif(ARCH STREQUAL "aarch64")
    # Future AArch64 configuration will go here
endif()

add_compile_options(${COMMON_FLAGS} ${ARCH_FLAGS})

# Apply flags to both the C compiler and the linker
#add_compile_options(${BARE_METAL_FLAGS})
set(CORE_SRCS
                external/flanterm/src/flanterm.c
                external/flanterm/src/flanterm_backends/fb.c
                src/main.cpp
                src/libc/memset.cpp
                src/libc/memcpy.cpp
                src/libc/strlen.cpp
                src/terminal.cpp
                src/drivers/pic.cpp
                src/drivers/keyboard.cpp
                src/kernel/String.cpp
                src/kernel/Shell.cpp
                src/kernel/cxxabi.cpp
                src/kernel/memory_operators.cpp
                src/kernel/memory.cpp
                src/utils/ring_buffer.cpp
                src/subsystems/input.cpp
                src/gui/Graphics.cpp
                src/drivers/mouse.cpp
                src/gui/Font.cpp
                src/gui/Window.cpp
                src/gui/Compositor.cpp
)

set(CORE_HEADERS
        # Explicitly listing headers helps CLion's code insight
        include/limine.h
        include/string.h
        include/terminal.h
        include/drivers/keyboard.h
        include/drivers/pic.h
        include/compiler.h
        include/kernel/shell.h
        include/kernel/String.hpp
        include/kernel/memory.hpp
        include/utils/ring_buffer.h
        include/subsystems/input.h
        include/gui/Graphics.h
        include/drivers/mouse.h
        include/gui/Font.h
        include/gui/Window.h
        include/gui/Compositor.h
        include/gui/cursor.h
        include/arch/cpu.h
)

add_executable(kernel.elf ${CORE_SRCS} ${ARCH_SRCS} ${CORE_HEADERS} ${ARCH_HEADERS})

set_target_properties(kernel.elf PROPERTIES LINKER_LANGUAGE CXX)
execute_process(
        COMMAND ${CMAKE_CXX_COMPILER} -print-resource-dir
        OUTPUT_VARIABLE CLANG_RESOURCE_DIR
        OUTPUT_STRIP_TRAILING_WHITESPACE
)
target_include_directories(kernel.elf PRIVATE
                            ${CMAKE_CURRENT_SOURCE_DIR}/include
                            external/flanterm/src
                            "${CLANG_RESOURCE_DIR}/include")

add_link_options(
    ${ARCH_LINK_FLAGS}
    "-static"
    "-T${CMAKE_CURRENT_SOURCE_DIR}/linker.ld"
)
set(CMAKE_CXX_LINK_EXECUTABLE "<CMAKE_LINKER> <FLAGS> <CMAKE_CXX_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")
target_link_options(kernel.elf PRIVATE
                    "-T${CMAKE_CURRENT_SOURCE_DIR}/linker.ld"
                    "-z" "max-page-size=0x1000"
                    "-no-pie")

# Find the host system tools needed to build an ISO image
find_program(XORRISO_PATH xorriso REQUIRED)

# Add a target to package the kernel and config into a bootable ISO image
add_custom_target(image
    DEPENDS kernel.elf
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/iso_root
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/kernel.elf ${CMAKE_CURRENT_BINARY_DIR}/iso_root/
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/limine.conf ${CMAKE_CURRENT_BINARY_DIR}/iso_root/
    
    # Download Limine bootloader binaries directly into the build environment
    COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/limine-binary/limine-bios.sys ${CMAKE_CURRENT_BINARY_DIR}/iso_root/limine-bios.sys
    COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/limine-binary/limine-bios-cd.bin ${CMAKE_CURRENT_BINARY_DIR}/iso_root/limine-bios-cd.bin
    # COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/limine-binary/limine ${CMAKE_CURRENT_BINARY_DIR}/iso_root/limine-deploy
    # COMMAND chmod +x ${CMAKE_CURRENT_BINARY_DIR}/iso_root/limine-deploy

    # Build the hybrid ISO image
    COMMAND xorriso -as mkisofs -R -r -J -b limine-bios-cd.bin
            -no-emul-boot -boot-load-size 4 -boot-info-table -hfsplus
            ${CMAKE_CURRENT_BINARY_DIR}/iso_root -o ${CMAKE_CURRENT_BINARY_DIR}/kernel.iso

    # Deploy Limine MBR to make the ISO bootable on both flash drives and CDs
    # COMMAND ${CMAKE_CURRENT_BINARY_DIR}/limine-deploy bios-install ${CMAKE_CURRENT_BINARY_DIR}/kernel.iso
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/limine-binary/limine bios-install ${CMAKE_CURRENT_BINARY_DIR}/kernel.iso
    COMMENT "Generating bootable image: build/kernel.iso"
)
