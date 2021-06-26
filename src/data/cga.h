
#ifndef CGA_H
#define CGA_H

#include "../convert.h"

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

#define CGA_HEADER_SZ                     20

#define CGA_HEADER_OFFSET_VERSION         2
#define CGA_HEADER_OFFSET_WIDTH           4
#define CGA_HEADER_OFFSET_HEIGHT          6
#define CGA_HEADER_OFFSET_BPP             8
#define CGA_HEADER_OFFSET_PLANE1_OFFSET   10
#define CGA_HEADER_OFFSET_PLANE1_SZ       12
#define CGA_HEADER_OFFSET_PLANE2_OFFSET   14
#define CGA_HEADER_OFFSET_PLANE2_SZ       16
#define CGA_HEADER_OFFSET_PALETTE         18

struct CGA_HEADER {
   uint16_t id;
   uint16_t version;
   uint16_t width;
   uint16_t height;
   uint16_t bpp;
   uint16_t plane1_offset;
   uint16_t plane1_sz;
   uint16_t plane2_offset;
   uint16_t plane2_sz;
   uint16_t palette;
};

uint32_t cga_write_plane(
   uint8_t*, uint32_t, const struct CONVERT_GRID* grid, int, int );
int cga_write_file(
   const char*, const struct CONVERT_GRID*, struct CONVERT_OPTIONS* );
int cga_write(
   uint8_t*, uint32_t, const struct CONVERT_GRID*, struct CONVERT_OPTIONS* );
struct CONVERT_GRID* cga_read_file(
   const char* path, struct CONVERT_OPTIONS* o );
struct CONVERT_GRID* cga_read(
   const uint8_t*, uint32_t, struct CONVERT_OPTIONS* );

#endif /* CGA_H */

