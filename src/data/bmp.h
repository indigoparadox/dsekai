
#ifndef BMP_H
#define BMP_H

#include "../convert.h"

int bmp_write( const char*, const struct CONVERT_GRID*, uint16_t );
struct CONVERT_GRID* bmp_read( const char* );

#endif /* BMP_H */

