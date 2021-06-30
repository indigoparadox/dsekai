
#ifndef ICO_H
#define ICO_H

#include "../../src/dstypes.h"

struct CONVERT_GRID;
struct CONVERT_OPTIONS;

#define ICO_TYPE_ICON   1
#define ICO_TYPE_CURSOR 2

struct
#ifdef __GNUC__
__attribute__( (__packed__) )
#endif /* __GNUC__ */
ICO_DIR_HEADER {
   uint16_t reserved;
   uint16_t type;
   uint16_t count;
};

struct
#ifdef __GNUC__
__attribute__( (__packed__) )
#endif /* __GNUC__ */
ICO_DIR_ENTRY_HEADER {
   uint8_t ico_w;
   uint8_t ico_h;
   uint8_t colors;
   uint8_t reserved;
   uint16_t ico_planes_cur_hotspot_h;
   uint16_t ico_bpp_cur_hotspot_v;
   uint32_t data_sz;
   uint32_t bmp_offset;
};

int ico_write_file(
   const char*, const struct CONVERT_GRID*, struct CONVERT_OPTIONS* );
int ico_write(
   uint8_t*, uint32_t, const struct CONVERT_GRID*, struct CONVERT_OPTIONS* );

#endif /* ICO_H */

