
#ifndef ICNS_H
#define ICNS_H

#include "../../src/dstypes.h"
#include "../convert.h"
#include "../../src/dio.h"

struct ICNS_FILE_HEADER {
   char     id[4];
   uint32_t file_sz;
};

struct ICNS_DATA_HEADER {
   char     icon_type[4];
   uint32_t data_sz;
};

DECLARE_FMT_READ( icns );
DECLARE_FMT_WRITE( icns );

#endif /* ICNS_H */

