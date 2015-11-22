#include "common.h"
#include "string.h"

char* bzero(char* dst, uint32_t count)
{
    uint32_t i;

    for (i = 0; i < count; i++)
    {
        dst[i] = 0;
    }
    return dst;
}

char* memcpy(char* dst, const char* src, uint32_t count)
{
    uint32_t i;

    for (i = 0; i < count; i++)
    {
        dst[i] = src[i];
    }
    return dst;
}

int32_t sprintf(char* str, const char* format, ...)
{
    int32_t i = 0;
    char c = format[i];

    for (i = 1; c != ASCII_NUL; i++)
    {
        if (c == '%')
        {
        }
        else
        {
            str[i-1] = c;
        }
        c = format[i];
    }
    return 0;
}
