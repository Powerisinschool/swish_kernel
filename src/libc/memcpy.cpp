#include <stddef.h>
#include <stdint.h>

/**
 * @brief Copies `count` characters (as if of type `unsigned char`) from the object pointed to by `src` into the object pointed to by `dest`.
 *
 * @param dest Pointer to the memory location to copy to.
 * @param src Pointer to the memory location to copy from.
 * @param count Number of bytes to copy.
 * @return A pointer to the memory destination, otherwise `dest`.
 */
extern "C" void *memcpy(void *dest, const void *src, size_t count)
{
    if (dest == nullptr || src == nullptr)
        return (dest);

    auto *d64 = static_cast<uint64_t *>(dest);
    auto s64 = static_cast<const uint64_t *>(src);

    while (count >= 32) {
        *d64++ = *s64++;
        *d64++ = *s64++;
        *d64++ = *s64++;
        *d64++ = *s64++;
        count -= 32;
    }

    const size_t chunks = count / 8; // max of 3 chunks
    for (size_t i = 0; i < chunks; i++) {
        *d64++ = *s64++;
    }

    auto *d8_rem = reinterpret_cast<uint8_t *>(d64);
    const auto *s8_rem = reinterpret_cast<const uint8_t *>(s64);

    const size_t remainder = count % 8;
    for (size_t i = 0; i < remainder; i++)
    {
        *d8_rem++ = *s8_rem++;
    }
    return (dest);
}
// Faster on bare metal, but slower with QEMU emulation due to emulation overhead
// extern "C" void *memcpy(void *dest, const void *src, size_t count)
// {
//     // Save the original destination pointer to return it later
//     void *original_dest = dest;
//
//     asm volatile (
//         "rep movsb"
//         : "+D" (dest), "+S" (src), "+c" (count) // Outputs / In-Outs
//         :                                       // Inputs (handled by In-Outs)
//         : "memory"                              // Clobbers: tells the compiler memory is modified
//     );
//
//     return original_dest;
// }
