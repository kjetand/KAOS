#include "common.h"
#include "string.h"

int8_t* bzero(int8_t *dst, int32_t count)
{
    int32_t i;

    for (i = 0; i < count; i++)
    {
        dst[i] = 0;
    }
    return dst;
}

int8_t* memcpy(int8_t *dst, int8_t *src, int32_t count)
{
    int32_t i;

    for (i = 0; i < count; i++)
    {
        dst[i] = src[i];
    }
    return dst;
}
