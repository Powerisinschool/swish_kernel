#include <stddef.h>
#include <stdint.h>

#include "string.h"

extern "C" void *memmove(void *dest, const void *src, size_t n)
{
    if (dest == nullptr || src == nullptr || dest == src || n == 0)
        return (dest);

    const auto d_addr = reinterpret_cast<uintptr_t>(dest);
    const auto s_addr = reinterpret_cast<uintptr_t>(src);

    if (d_addr < s_addr || d_addr >= (s_addr + n)) {
        memcpy(dest, src, n);
    } else {
        auto d64 = reinterpret_cast<uint64_t *>(d_addr + n - 8);
        auto s64 = reinterpret_cast<uint64_t *>(s_addr + n - 8);

        while (n >= 32) {
            *d64-- = *s64--;
            *d64-- = *s64--;
            *d64-- = *s64--;
            *d64-- = *s64--;
            n -= 32;
        }

        // Should run a max of 3 times
        while (n >= 8) {
            *d64-- = *s64--;
            n -= 8;
        }

        // handle leftover bytes
        auto d8 = reinterpret_cast<uint8_t *>(d_addr + n - 1);
        auto s8 = reinterpret_cast<uint8_t *>(s_addr + n - 1);

        while (n > 0) {
            *d8-- = *s8--;
            n -= 1;
        }
    }

    return (dest);
}
