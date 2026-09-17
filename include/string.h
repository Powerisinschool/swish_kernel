#pragma once
#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif

    void *memset(void *dest, int ch, size_t count);
    void *memmove(void *dest, const void *src, size_t n);
    void *memcpy(void *dest, const void *src, size_t count);
    size_t strlen(const char *str);
    char *strncpy(char *dest, const char *src, size_t n);

#ifdef __cplusplus
}
#endif
