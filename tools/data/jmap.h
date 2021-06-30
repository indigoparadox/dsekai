
#ifndef JMAP_H
#define JMAP_H

#include "../../src/dstypes.h"
#include "../convert.h"

struct CONVERT_GRID* jmap_read_file(
   const char*, struct CONVERT_OPTIONS* );
struct CONVERT_GRID* jmap_read(
   const uint8_t*, uint32_t, struct CONVERT_OPTIONS* );

#endif /* JMAP_H */

