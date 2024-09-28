#ifndef WEUTIL_H
#define WEUTIL_H

#include <stdlib.h>

char * WEstring_new(const char *format, ... );

void we_log(const char *format, ... );

#define WE_FREE_AND_NULL(p) do { free(p); p = NULL; } while(0);

#endif