
#ifndef DRC_H
#define DRC_H

#include "../dstypes.h"

#define DRC_MAX_PATH 255
#ifdef USE_DOS
#define DRC_DEFAULT_TEMP "c:\\temp"
#define DRC_PATH_SEP '\\'
#else
#define DRC_DEFAULT_TEMP "/tmp"
#define DRC_PATH_SEP '/'
#endif /* USE_DOS */

#define DRC_ERROR_PATH_TOO_LONG           -1
#define DRC_ERROR_COULD_NOT_CREATE        -2
#define DRC_ERROR_COULD_NOT_CREATE_TEMP   -3
#define DRC_ERROR_COULD_NOT_OPEN          -4
#define DRC_ERROR_COULD_NOT_READ          -5
#define DRC_ERROR_COULD_NOT_WRITE         -6

#define DRC_HEADER_SZ                     30
#define DRC_HEADER_OFFSET_TYPE            0
#define DRC_HEADER_OFFSET_VERSION         4
#define DRC_HEADER_OFFSET_CRC32           6
#define DRC_HEADER_OFFSET_FILESIZE        10
#define DRC_HEADER_OFFSET_TOC_START       14
#define DRC_HEADER_OFFSET_TOC_ENTRIES     18
/* Reserved: 4 bytes. */
#define DRC_HEADER_OFFSET_FES             26

struct DRC_HEADER {
   uint32_t type;
   uint32_t version;
   uint32_t crc32; /* TODO */
   uint32_t filesize;
   uint32_t toc_start;
   uint32_t toc_entries;
   uint32_t first_entry_start;
};

struct DRC_TOC_E {
   uint32_t type;
   uint32_t id;
   uint32_t data_start;
   uint32_t data_sz;
   uint16_t name_len;
   char* name;          /* Used by packer/preprocessor to map image to ID. */
   uint8_t* data;
};

int32_t drc_list_resources( const char*, struct DRC_TOC_E** );
int32_t drc_create( const char* );
int32_t drc_add_resource( const char*, uint32_t, uint32_t, const char*,
   uint16_t, const uint8_t*, uint32_t );
int32_t drc_remove_resource( const char*, uint32_t, uint32_t );
int32_t drc_get_resource( const char*, uint32_t, uint32_t, struct DRC_TOC_E* );
int32_t drc_get_end( const char* );

#endif /* DRC_H */

