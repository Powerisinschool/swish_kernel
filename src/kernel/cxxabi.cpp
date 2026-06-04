extern "C" {
    // Registers destructors for static objects. We do nothing because the kernel never exits.
    int __cxa_atexit(void (*destructor)(void *), void *arg, void *dso_handle) {
        return 0;
    }

    // Called if a pure virtual function is somehow called.
    // You should ideally trigger a kernel panic here.
    void __cxa_pure_virtual() {
        while (1) {}
    }

    // Thread-safe initialization of local statics.
    // Since you likely don't have SMP (multi-threading) yet, a basic implementation works.
    int __cxa_guard_acquire(long *guard_object) {
        // If the first byte is 0, the object hasn't been initialized
        if (*reinterpret_cast<char*>(guard_object) == 0) {
            return 1; // Return 1 to proceed with initialization
        }
        return 0; // Already initialized
    }

    void __cxa_guard_release(long *guard_object) {
        // Mark as initialized
        *reinterpret_cast<char*>(guard_object) = 1;
    }

    void __cxa_guard_abort(long *guard_object) {
        // Do nothing for now
    }
}
