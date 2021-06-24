
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
#define DRC_ERROR_COULD_NOT_MOVE_TEMP     -7
#define DRC_ERROR_BAD_VERSION             -8
#define DRC_ERROR_BAD_BUFFER              -9
#define DRC_ERROR_COULD_NOT_ALLOC         -10

#define DRC_HEADER_SZ                     30
#define DRC_HEADER_OFFSET_TYPE            0
#define DRC_HEADER_OFFSET_VERSION         4
#define DRC_HEADER_OFFSET_CRC32           6
#define DRC_HEADER_OFFSET_FILESIZE        10
#define DRC_HEADER_OFFSET_TOC_START       14
#define DRC_HEADER_OFFSET_TOC_ENTRIES     18
/* Reserved: 4 bytes. */
#define DRC_HEADER_OFFSET_FES             26

#define DRC_TOC_E_SZ                      22
#define DRC_TOC_E_OFFSET_TYPE             0
/* Reserved: 4 byte. */
#define DRC_TOC_E_OFFSET_ID               8
#define DRC_TOC_E_OFFSET_DATA_START       12
#define DRC_TOC_E_OFFSET_DATA_SZ          16
#define DRC_TOC_E_OFFSET_NAME_SZ          20
#define DRC_TOC_E_OFFSET_NAME             22

struct DRC_HEADER {
   uint32_t type;
   uint16_t version;
   uint32_t crc32; /* TODO */
   uint32_t filesize;
   uint32_t toc_start;
   uint32_t toc_entries;
   uint32_t first_entry_start;
};

struct DRC_TOC_E {
   uint32_t type;
   uint32_t reserved; /* NULL byte for type to emulate a string. */
   uint32_t id;
   uint32_t data_start;
   uint32_t data_sz;
   uint16_t name_sz;
   char* name;          /* Used by packer/preprocessor to map image to ID. */
   uint8_t* data;
};

int32_t drc_list_resources( const char*, struct DRC_TOC_E** );
int32_t drc_create( const char* );
int32_t drc_add_resource( const char*, uint32_t, uint32_t, const char*,
   uint16_t, const uint8_t*, uint32_t );
int32_t drc_remove_resource( const char*, uint32_t, uint32_t );
int32_t drc_get_resource( const char*, uint32_t, uint32_t, uint8_t** );
int32_t drc_get_end( const char* );
int32_t drc_get_resource_sz( const char*, uint32_t, uint32_t );
int32_t drc_get_resource_name( const char*, uint32_t, uint32_t, char** );

#endif /* DRC_H */

