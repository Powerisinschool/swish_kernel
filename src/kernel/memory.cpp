#include <limine.h>
#include <kernel/memory.hpp>
#include <terminal.h>

static volatile struct limine_memmap_request memmap_request = {
    .id = LIMINE_MEMMAP_REQUEST_ID,
    .revision = 0
};

void initialize_physical_memory() {
    // Check if Limine actually provided the response
    if (memmap_request.response == nullptr) {
        k_print("PANIC: Limine did not provide a memory map!\r\n");
        while (true) {} // Halt
    }

    struct limine_memmap_response *memmap = memmap_request.response;

    uintptr_t highest_base = 0;
    size_t largest_size = 0;

    // 2. Iterate over all memory regions provided by the bootloader
    for (uint64_t i = 0; i < memmap->entry_count; i++) {
        struct limine_memmap_entry *entry = memmap->entries[i];

        // 3. We only care about RAM that is completely free to use
        if (entry->type == LIMINE_MEMMAP_USABLE) {

            // For a simple single-heap setup, just grab the largest available chunk.
            // (A production OS would link all usable chunks into physical page frames).
            if (entry->length > largest_size) {
                largest_size = entry->length;
                highest_base = entry->base;
            }
        }
    }

    // 4. Initialize our linked-list heap with the physical memory Limine found
    if (largest_size > 0) {
        init_heap(highest_base, largest_size);
        k_print("Heap initialized successfully with Limine data.\r\n");
    } else {
        k_print("PANIC: No usable memory found by Limine!\r\n");
        while (true) {} // Halt
    }
}

// The hidden metadata header placed right before the actual memory given to the user
struct MemoryBlock {
    size_t size;           // Size of the usable memory (excluding this header)
    bool is_free;          // Is this block available?
    MemoryBlock* next;     // Pointer to the next block in the heap
};

// Pointer to the very first block in our heap
static MemoryBlock* free_list_head = nullptr;

// Helper to align sizes to 8 bytes
static inline size_t align_8(size_t size) {
    return (size + 7) & ~7;
}

void init_heap(uintptr_t heap_start, size_t heap_size) {
    // We need at least enough room for one header
    if (heap_size < sizeof(MemoryBlock)) return;

    // Place the first header at the very beginning of our heap space
    free_list_head = reinterpret_cast<MemoryBlock*>(heap_start);

    // The usable size is the total size minus the size of this header
    free_list_head->size = heap_size - sizeof(MemoryBlock);
    free_list_head->is_free = true;
    free_list_head->next = nullptr;
}

extern "C" void* kmalloc(size_t size) {
    if (size == 0) return nullptr;

    size_t aligned_size = align_8(size);
    MemoryBlock* current = free_list_head;

    // Traverse the linked list looking for a free block that is big enough
    while (current != nullptr) {
        if (current->is_free && current->size >= aligned_size) {

            // If the block is big enough to be split into two usable blocks, do it.
            // We need enough leftover space for another header + at least 8 bytes of data.
            if (current->size >= aligned_size + sizeof(MemoryBlock) + 8) {

                // Calculate where the new split block should start
                uintptr_t next_block_addr = reinterpret_cast<uintptr_t>(current) + sizeof(MemoryBlock) + aligned_size;
                MemoryBlock* new_block = reinterpret_cast<MemoryBlock*>(next_block_addr);

                // Set up the new block's header
                new_block->size = current->size - aligned_size - sizeof(MemoryBlock);
                new_block->is_free = true;
                new_block->next = current->next;

                // Update the current block to reflect its new, smaller size
                current->size = aligned_size;
                current->next = new_block;
            }

            // Mark the block as used and return the memory right AFTER the header
            current->is_free = false;
            return reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(current) + sizeof(MemoryBlock));
        }
        current = current->next;
    }

    // Out of memory
    return nullptr;
}

extern "C" void kfree(void* ptr) {
    if (ptr == nullptr) return;

    // Step backward in memory to find the hidden header
    MemoryBlock* block = reinterpret_cast<MemoryBlock*>(reinterpret_cast<uintptr_t>(ptr) - sizeof(MemoryBlock));
    block->is_free = true;

    // Coalescing (Merging): Prevent memory fragmentation.
    // Scan the list to merge adjacent free blocks back into larger blocks.
    MemoryBlock* current = free_list_head;
    while (current != nullptr && current->next != nullptr) {
        if (current->is_free && current->next->is_free) {
            // Merge the two blocks by skipping the next header and adding its size
            current->size += sizeof(MemoryBlock) + current->next->size;
            current->next = current->next->next;
        } else {
            current = current->next;
        }
    }
}
