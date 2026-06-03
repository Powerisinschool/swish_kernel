#pragma once

/// For placing metadata/handshake requests into specific linker sections
#define LINK_SECTION(name) __attribute__((section(name)))

/// Ensures the linker doesn't optimize away the variable, even if unused in C++
#define KEEP_SYMBOL __attribute__((used))

/// Standard modern C++ replacement for [[maybe_unused]]/volatile if combined
#define KERNEL_REQUEST KEEP_SYMBOL LINK_SECTION(".limine_requests") [[maybe_unused]]
