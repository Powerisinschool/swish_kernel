#include <kernel/memory.hpp>

// Forward declarations for your kernel heap allocator.
// Replace these with your actual heap allocation functions if you have them.
// extern "C" void* kmalloc(size_t size);
// extern "C" void kfree(void* ptr);

// Note: If you don't have kmalloc/kfree yet, you can temporarily return nullptr,
// but be aware that any code calling 'new' will immediately crash your kernel.

void* operator new(size_t size) {
    return kmalloc(size);
}

void* operator new[](size_t size) {
    return operator new(size);
}

void operator delete(void* p) {
    kfree(p);
}

void operator delete(void* p, size_t size) {
    operator delete(p);
}

void operator delete[](void* p) {
    operator delete(p);
}

void operator delete[](void* p, size_t size) {
    operator delete(p);
}
