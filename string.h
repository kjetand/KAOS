#ifndef _STRING_H_
#define _STRING_H_

#include "common.h"

char* bzero(char* dst, uint32_t count);
char* memcpy(char* dst, const char* src, uint32_t count);
int32_t sprintf(char* str, const char* format, ...);

#endif
