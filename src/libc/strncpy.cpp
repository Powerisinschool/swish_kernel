#include <stddef.h>

extern "C" char *strncpy(char *dest, const char *src, size_t n) {
    for (size_t i = 0; i < n; i++) {
        dest[i] = src[i];
        if (src[i] == '\0') {
            while (i < n - 1) {
                i++;
                dest[i] = '\0';
            }
        }
    }
    return (dest);
}
