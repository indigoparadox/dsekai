
#ifndef CGA_H
#define CGA_H

#include "../convert.h"

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

int cga_write( const char* path, const struct CONVERT_GRID* grid );
struct CONVERT_GRID* cga_read( const char* path, int sz_x, int sz_y );

#endif /* CGA_H */

