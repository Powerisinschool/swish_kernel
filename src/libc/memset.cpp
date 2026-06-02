#include <stddef.h>

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
    unsigned char *d = static_cast<unsigned char *>(dest);
    unsigned char value = static_cast<unsigned char>(ch);

    for (size_t i = 0; i < count; i++)
    {
        d[i] = value;
    }

    return (dest);
}
