#include <stddef.h>

/**
 * @brief Calculate the length of a string.
 *
 * @param str The string to calculate the length of.
 * @return The length of the string.
 */
// extern "C" size_t strlen(const char *str)
// {
//     size_t len = 0;
//     while (str[len] != '\0')
//     {
//         len++;
//     }
//     return len;
// }
extern "C" size_t strlen(const char *str)
{
    size_t length;

    asm volatile (
        "mov $-1, %%rcx\n\t"     // Set rcx to maximum possible value (all 1s)
        "xor %%al, %%al\n\t"     // Set al to 0 (the null terminator we are looking for)
        "repne scasb\n\t"        // Scan memory at rdi for al, decrementing rcx each time
        "not %%rcx\n\t"          // Invert rcx to get the count
        "dec %%rcx\n\t"          // Subtract 1 to exclude the null terminator itself
        : "=c" (length), "+D" (str) // Outputs: length goes to rcx, str goes to rdi
        :                           // Inputs: (handled in outputs)
        : "al", "memory"            // Clobbers: al register and memory
    );

    return length;
}
