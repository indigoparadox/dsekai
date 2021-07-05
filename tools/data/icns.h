
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

int icns_write( struct DIO_STREAM*, MEMORY_HANDLE, struct CONVERT_OPTIONS* );
MEMORY_HANDLE icns_read( struct DIO_STREAM*, struct CONVERT_OPTIONS* );
#endif /* ICNS_H */

