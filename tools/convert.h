
#ifndef CONVERT_H
#define CONVERT_H

#include "../src/dstypes.h"
#include "../src/dio.h"

#define CONVERT_BUFFER_SZ 1024

#define CONVERT_ERROR_FILE_WRITE    -1

#define CONVERT_GRID_TYPE 0x43565447

#define DECLARE_FMT_READ( fmt ) int32_t fmt ## _read( struct DIO_STREAM* stream, MEMORY_HANDLE* grid_handle, struct CONVERT_OPTIONS* o )

#define DECLARE_FMT_WRITE( fmt ) int32_t fmt ## _write( struct DIO_STREAM* stream, MEMORY_HANDLE grid_handle, struct CONVERT_OPTIONS* o )

#define GRID_DATA_PTR( grid_in ) (uint8_t*)((grid_in) + sizeof( struct CONVERT_GRID_HEADER ));

#define LOCK_CONVERT_GRID( data_in, grid_in, handle_in ) grid_in = memory_lock( (handle_in) ); if( NULL == (grid_in) ) { goto cleanup; } (data_in) = GRID_DATA_PTR( grid_in );

#define NEW_CONVERT_GRID( w_in, h_in, bpp_in, data_in, grid_in, handle_in ) (handle_in) = memory_alloc( 1, sizeof( struct CONVERT_GRID_HEADER ) + (w_in * h_in) ); if( NULL == (handle_in) ) { goto cleanup; } LOCK_CONVERT_GRID( data_in, grid_in, handle_in ); (grid_in)->type = CONVERT_GRID_TYPE; (grid_in)->version = 1; (grid_in)->data_sz = ((w_in) * (h_in)); (grid_in)->sz_x = (w_in); (grid_in)->sz_y = (h_in); (grid_in)->bpp = (bpp_in);

struct CONVERT_GRID_HEADER {
   uint32_t type;
   uint32_t version;
   int32_t sz_x;
   int32_t sz_y;
   uint32_t bpp;
   uint32_t data_sz;
};

struct CONVERT_OPTIONS {
   uint8_t reverse;
   uint16_t bpp;
   int32_t w;
   int32_t h;
   uint32_t line_padding;
   uint32_t plane_padding;
   uint32_t bmp_data_sz;
   uint8_t cga_use_header;
   uint8_t bmp_no_file_header;
   uint32_t bmp_data_offset_out;
};

#endif /* CONVERT_H */

