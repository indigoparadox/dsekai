
#ifndef MAPTOH
#define MAPTOH

#include "../../src/dstypes.h"

#include "../convert.h"

int header_map_write_file(
   const char*, const char*, struct CONVERT_OPTIONS* );
int header_map_write(
   uint8_t*, uint32_t, uint8_t*, uint32_t, struct CONVERT_OPTIONS* );

#endif /* MAPTOH */

