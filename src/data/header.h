
#ifndef HEADER_H
#define HEADER_H

#include "../dstypes.h"
#include "../convert.h"

int header_write_file(
   const char*, const struct CONVERT_GRID*, struct CONVERT_OPTIONS* );
int header_write(
   uint8_t*, uint32_t, const struct CONVERT_GRID*, struct CONVERT_OPTIONS* );

#endif /* HEADER_H */

