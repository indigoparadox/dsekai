
#ifndef CGA_H
#define CGA_H

#include "../convert.h"

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

int cga_write( const char*, const struct CONVERT_GRID*, int );
struct CONVERT_GRID* cga_read( const char*, int, int, int );

#endif /* CGA_H */

