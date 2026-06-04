#pragma once
#include <stddef.h>
#include <stdint.h>

// Initialize the heap with a starting physical address and total size
void init_heap(uintptr_t heap_start, size_t heap_size);

// C-linkage so our global operator new/delete can easily hook into these
extern "C" void* kmalloc(size_t size);
extern "C" void kfree(void* ptr);
