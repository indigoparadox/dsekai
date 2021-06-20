
#ifndef PSPRINTF_H
#define PSPRINTF_H

/*
Wrote this for PalmOS, but might be useful for other platforms without
snprinf.
*/

#include <stdarg.h>

#define PSITOA_BUF_LEN 10

int psitoa( char* buffer, int buffer_len, int d, int d_base );
int psprintf( char* buffer, int buffer_len, const char* fmt, ... );

#endif /* PSPRINTF_H */

