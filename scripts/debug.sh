#!/usr/bin/env bash

export BINARY=build/kernel.elf

/opt/homebrew/opt/llvm/bin/llvm-objdump -d -h $BINARY
file $BINARY
