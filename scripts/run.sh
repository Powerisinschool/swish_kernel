#!/usr/bin/env bash

export IMAGE=build/kernel.iso

qemu-system-x86_64 \
  -cdrom $IMAGE \
  -boot d \
  -m 512M \
  -net none \
  -serial stdio
