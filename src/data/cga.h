
#ifndef CGA_H
#define CGA_H

#include "../convert.h"

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

uint32_t cga_write_plane(
   uint8_t*, uint32_t, const struct CONVERT_GRID* grid, int, int );
int cga_write_file(
   const char*, const struct CONVERT_GRID*, struct CONVERT_OPTIONS* );
int cga_write(
   uint8_t*, uint32_t, const struct CONVERT_GRID*, struct CONVERT_OPTIONS* );
struct CONVERT_GRID* cga_read_file(
   const char* path, struct CONVERT_OPTIONS* o );
struct CONVERT_GRID* cga_read( uint8_t*, uint32_t, struct CONVERT_OPTIONS* );

#endif /* CGA_H */

