#!/usr/bin/env bash

cmake --build build --target kernel.elf

export BINARY=build/kernel.elf

/opt/homebrew/opt/llvm/bin/llvm-objdump -d -h $BINARY
file $BINARY
