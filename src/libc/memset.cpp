#include <stddef.h>
#include <stdint.h>

/**
 * @brief Copies the value `static_cast<unsigned char>(ch)` into each of the first `count` characters of the object pointed to by `dest`.
 *
 * @param dest Pointer to the object to fill.
 * @param ch Fill byte.
 * @param count Number of bytes to fill.
 * @return A pointer to the memory destination.
 */
extern "C" void *memset(void *dest, int ch, size_t count)
{
    if (dest == nullptr) return (dest);
    auto *d64 = static_cast<uint64_t *>(dest);
    const auto value = static_cast<unsigned char>(ch);
    const auto v64 = static_cast<uint64_t>(ch & 0xFF) * 0x0101010101010101ULL;

    while (count >= 32) {
        *d64++ = v64;
        *d64++ = v64;
        *d64++ = v64;
        *d64++ = v64;
        count -= 32;
    }

    const size_t chunks = count / 8;
    for (size_t i = 0; i < chunks; i++) {
        *d64++ = v64;
    }

    auto *d8_rem = reinterpret_cast<uint8_t *>(d64);

    for (size_t i = 0; i < count % 8; i++)
    {
        *d8_rem++ = value;
    }

    return (dest);
}
// Faster on bare metal, but slower with QEMU emulation due to emulation overhead
// extern "C" void *memset(void *dest, int ch, size_t count)
// {
//     void *original_dest = dest;
//
//     asm volatile (
//         "rep stosb"
//         : "+D" (dest), "+c" (count)             // Outputs / In-Outs (rdi, rcx)
//         : "a" (ch)                              // Input: put 'ch' into the 'al' register
//         : "memory"                              // Tells compiler memory is modified
//     );
//
//     return original_dest;
// }
