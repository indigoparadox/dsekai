
#ifndef BMP_H
#define BMP_H

#include "../../src/dstypes.h"

struct CONVERT_GRID;
struct CONVERT_OPTIONS;

struct
#ifdef __GNUC__
__attribute__( (__packed__) )
#endif /* __GNUC__ */
BITMAP_FILE_HEADER {
   char id[2];
   uint32_t file_sz;
   uint16_t reserved1;
   uint16_t reserved2;
   uint32_t bmp_offset;
};

struct
#ifdef __GNUC__
__attribute__( (__packed__) )
#endif /* __GNUC__ */
BITMAP_DATA_HEADER {
   uint32_t header_sz;
   int32_t bitmap_w;
   int32_t bitmap_h;
   uint16_t planes;
   uint16_t bpp;
   uint32_t compression;
   uint32_t image_sz;
   int32_t hres;
   int32_t vres;
   uint32_t colors;
   uint32_t colors_important;
};

#define BMP_COMPRESSION_NONE (0)

int bmp_write_file(
   const char*, const struct CONVERT_GRID*, struct CONVERT_OPTIONS* );
int bmp_write(
   uint8_t*, uint32_t, const struct CONVERT_GRID*, struct CONVERT_OPTIONS* );
struct CONVERT_GRID* bmp_read_file(
   const char* path, struct CONVERT_OPTIONS* o );
struct CONVERT_GRID* bmp_read(
   const uint8_t*, uint32_t, struct CONVERT_OPTIONS* );

#endif /* BMP_H */

