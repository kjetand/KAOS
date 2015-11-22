#include "common.h"
#include "string.h"

static int32_t format_hex(char* str, uint32_t n, bool_t upper_case);

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
    char*   curr = str;
    int32_t i = 0;
    char    c = format[i];

    for (i = 1; c != ASCII_NUL; i++)
    {
        if (c == '%')
        {
            c = format[i];

            switch (c)
            {
            case 'i':
            case 'd':
                break;
            case 'u':
                break;
            case 'x':
                curr += format_hex(curr, 1234, FALSE);
                i++;
                break;
            case 'X':
                curr += format_hex(curr, 1234, TRUE);
                i++;
                break;
            default:
                *curr++ = '%';
            }
        }
        else
        {
            *curr++ = c;
        }
        c = format[i];
    }
    return 0;
}

static int32_t format_hex(char* str, uint32_t n, bool_t upper_case)
{
    static const char uc_digits[BASE_HEXADECIMAL] = "0123456789ABCDEF";
    static const char lc_digits[BASE_HEXADECIMAL] = "0123456789abcdef";

    int32_t count = 0;
    int8_t i;
    

    for (i = 7; i >= 0; i--)
    {
        if (upper_case)
        {
            str[i] = uc_digits[n & 0xF];
        }
        else
        {
            str[i] = lc_digits[n & 0xF];
        }
        n = n >> 4;
        count++;
    }
    return count;
}
