
/* \file drc.h */

#ifndef DRC_H
#define DRC_H

#ifndef DRC_COPY_BLOCK_SZ
#define DRC_COPY_BLOCK_SZ 1024
#endif /* !DRC_COPY_BLOCK_SZ */

#ifdef USE_DOS
#define DRC_DEFAULT_TEMP "c:\\temp"
#define DRC_PATH_SEP '\\'
#else
#define DRC_DEFAULT_TEMP "/tmp"
#define DRC_PATH_SEP '/'
#endif /* USE_DOS */

#define DRC_FILENAME_SZ 64

/* This is kind of arbitrary. Test if increasing. */
#define DRC_MAX_ENTRIES 1024

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
#define DRC_ERROR_COULD_NOT_FIND          -11

#define DRC_TYPE_STR( type, a, b, c, d ) type.str[0] = 'a'; type.str[1] = 'b'; type.str[2] = 'c'; type.str[3] = 'd';

union DRC_TYPE {
   char str[4];
   uint32_t u32;
};

struct
#ifdef __GNUC__
__attribute__( (__packed__) )
#endif /* __GNUC__ */
DRC_HEADER {
   union DRC_TYPE type;
   uint32_t version;
   uint32_t crc32; /* TODO */
   uint32_t filesize;
   uint32_t toc_start;
   uint32_t toc_entries;
   uint32_t reserved;
   uint32_t first_entry_start;
};

struct
#ifdef __GNUC__
__attribute__( (__packed__) )
#endif /* __GNUC__ */
DRC_TOC_E {
   union DRC_TYPE type;
   uint32_t reserved; /* NULL byte for type to emulate a string. */
   uint32_t id;
   uint32_t data_start;
   uint32_t data_sz;
   uint32_t name_sz;
   char name[DRC_FILENAME_SZ];
};

int32_t drc_list_resources( struct DIO_STREAM*, MEMORY_HANDLE*, uint16_t );
int32_t drc_get_resource(
   struct DIO_STREAM*, union DRC_TYPE, uint32_t, MEMORY_PTR, uint16_t );
int32_t drc_get_resource_sz( struct DIO_STREAM*, union DRC_TYPE, uint32_t );
int32_t drc_get_resource_name(
   struct DIO_STREAM*, union DRC_TYPE, uint32_t, char** );

#endif /* DRC_H */

