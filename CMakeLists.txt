cmake_minimum_required(VERSION 4.2)

# Force a freestanding, bare-metal environment configuration
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

# Tell CMake not to look for standard system libraries or compiler runtimes
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

project(BareMetalKernel CXX ASM)

# Define the Clang cross-compilation flags
set(BARE_METAL_FLAGS
    "-target" "x86_64-unknown-none-elf"
    "-ffreestanding"
    "-mcmodel=kernel"
    "-mno-red-zone"
    "-fno-exceptions"
    "-fno-rtti"
    "-fno-pic"
    "-fno-pie"
    "-mno-sse"
    "-mno-sse2"
    "-mno-mmx"
    "-mno-80387"
    "-mgeneral-regs-only"
    "-O3"
    "-fomit-frame-pointer"
)

# Apply flags to both the C compiler and the linker
add_compile_options(${BARE_METAL_FLAGS})
add_executable(kernel.elf src/main.cpp)

set_target_properties(kernel.elf PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(kernel.elf PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
add_link_options(
    "-m" "elf_x86_64"
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
