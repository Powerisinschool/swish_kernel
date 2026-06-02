#include <stddef.h>

/**
 * @brief Calculate the length of a string.
 * 
 * @param str The string to calculate the length of.
 * @return The length of the string.
 */
extern "C" size_t strlen(const char *str)
{
    size_t len = 0;
    while (str[len] != '\0')
    {
        len++;
    }
    return len;
}
