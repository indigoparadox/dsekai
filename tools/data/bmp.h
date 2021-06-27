
#ifndef BMP_H
#define BMP_H

#include "../../src/dstypes.h"

struct CONVERT_GRID;
struct CONVERT_OPTIONS;

int bmp_write_file(
   const char*, const struct CONVERT_GRID*, struct CONVERT_OPTIONS* );
int bmp_write(
   uint8_t*, uint32_t, const struct CONVERT_GRID*, struct CONVERT_OPTIONS* );
struct CONVERT_GRID* bmp_read_file(
   const char* path, struct CONVERT_OPTIONS* o );
struct CONVERT_GRID* bmp_read(
   const uint8_t*, uint32_t, struct CONVERT_OPTIONS* );

#endif /* BMP_H */

