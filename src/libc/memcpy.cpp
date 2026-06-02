#include <stddef.h>

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

    unsigned char *d = static_cast<unsigned char *>(dest);
    const unsigned char *s = static_cast<const unsigned char *>(src);

    for (size_t i = 0; i < count; i++)
    {
        d[i] = s[i];
    }
    return (dest);
}
