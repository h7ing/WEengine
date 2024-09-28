#include "weutil.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>


char * WEstring_new(const char *fmt, ... ) {
    va_list args;
    va_start(args, fmt);
    char buffer[512];
    vsnprintf(buffer, 512, fmt, args);
    va_end(args);

    int strsz = strlen(buffer) + 1;
    char *str = malloc(strsz);
    strcpy(str, buffer);

    return str;
}


void we_log(const char *format, ... ) {
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}