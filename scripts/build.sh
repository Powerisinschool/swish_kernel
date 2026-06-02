#!/usr/bin/env bash

rm -rf build

# For macOS Homebrew LLVM (adjust path if using Linux /usr/bin/ld.lld)
LLVM_PATH="/opt/homebrew/opt/llvm/bin"
LD_PATH="/opt/homebrew/opt/lld/bin"

cmake -B build -S . \
  -DCMAKE_CXX_COMPILER="${LLVM_PATH}/clang++" \
  -DCMAKE_ASM_COMPILER="${LLVM_PATH}/clang" \
  -DCMAKE_LINKER="${LD_PATH}/ld.lld"

# Compile
cmake --build build --target image
