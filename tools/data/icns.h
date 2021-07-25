
#ifndef ICNS_H
#define ICNS_H

#include "../convert.h"

#define ICNS_FILE_HEADER_SZ               8

#define ICNS_FILE_HEADER_OFFSET_ID        0
#define ICNS_FILE_HEADER_OFFSET_FILE_SZ   4

#define ICNS_DATA_HEADER_SZ               8

#define ICNS_DATA_HEADER_OFFSET_ICON_TYPE 0   
#define ICNS_DATA_HEADER_OFFSET_DATA_SZ   4

struct ICNS_FILE_HEADER {
   char     id[4];
   uint32_t file_sz;
};

struct ICNS_DATA_HEADER {
   char     icon_type[4];
   uint32_t data_sz;
};

int icns_write_file(
   const char*, const struct CONVERT_GRID*, struct CONVERT_OPTIONS* );
int icns_write(
   uint8_t*, uint32_t, const struct CONVERT_GRID*, struct CONVERT_OPTIONS* );
struct CONVERT_GRID* icns_read_file(
   const char* path, struct CONVERT_OPTIONS* o );
struct CONVERT_GRID* icns_read(
   const uint8_t*, uint32_t, struct CONVERT_OPTIONS* );
#endif /* ICNS_H */

