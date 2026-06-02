#pragma once
#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif

    void *memset(void *dest, int ch, size_t count);
    void *memcpy(void *dest, const void *src, size_t count);
    size_t strlen(const char *str);

#ifdef __cplusplus
}
#endif
