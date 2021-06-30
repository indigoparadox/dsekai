
#include "ico.h"

#include <stdio.h>

#include "../../src/memory.h"
#include "bmp.h"
#include "../convert.h"

int32_t ico_write_file(
   const char* path, const struct CONVERT_GRID* grid, struct CONVERT_OPTIONS* o
) {
   int32_t ico_buffer_sz = 0,
      ico_row_sz = 0,
      palette_entries = 0,
      ico_file_sz = 0,
      written = 0;
   uint8_t* ico_buffer = NULL;
   FILE* file_out = NULL;

   /* Add rows padded out to 4 bytes. */
   ico_row_sz = (grid->sz_x * o->bpp) / 8;
   if( 0 != ico_row_sz % 4 ) {
      ico_row_sz += (4 - (ico_row_sz % 4));
   }
   palette_entries = bmp_colors_count( o->bpp );

   ico_buffer_sz = 
      sizeof( struct ICO_DIR_HEADER ) +
      sizeof( struct ICO_DIR_ENTRY_HEADER ) +
      sizeof( struct BITMAP_DATA_HEADER ) +
      (4 * palette_entries) + /* Palette entries are 32-bit (4 bytes). */
      (grid->sz_y * ico_row_sz) + /* Data */
      (grid->sz_y * ico_row_sz); /* Mask */

   ico_buffer = memory_alloc( 1, ico_buffer_sz );
   assert( NULL != ico_buffer );

   ico_file_sz = ico_write( ico_buffer, ico_buffer_sz, grid, o );
   if( 0 >= ico_file_sz ) {
      memory_free( &ico_buffer );
      return ico_file_sz;
   }
   assert( ico_buffer_sz >= ico_file_sz );

   file_out = fopen( path, "wb" );
   assert( NULL != file_out );

   written = fwrite( ico_buffer, 1, ico_buffer_sz, file_out );

   fclose( file_out );
   memory_free( &ico_buffer );

   if( written != ico_buffer_sz ) {
      return CONVERT_ERROR_FILE_WRITE;
   }

   return ico_file_sz;
}

int32_t ico_write(
   uint8_t* buf_ptr, uint32_t buf_sz,
   const struct CONVERT_GRID* grid, struct CONVERT_OPTIONS* o
) {
   int32_t bmp_data_offset = 0,
      bmp_file_sz = 0,
      bmp_mask_sz = 0,
      bmp_mask_offset = 0,
      i = 0;
   struct ICO_DIR_HEADER* dir_header = (struct ICO_DIR_HEADER*)&(buf_ptr[0]);
   struct ICO_DIR_ENTRY_HEADER* dir_entry_header = NULL;
   struct BITMAP_DATA_HEADER* bmp_header = NULL;

   dir_entry_header =
      (struct ICO_DIR_ENTRY_HEADER*)&(buf_ptr[sizeof( struct ICO_DIR_HEADER )]);

   bmp_data_offset = 
      sizeof( struct ICO_DIR_HEADER ) +
      sizeof( struct ICO_DIR_ENTRY_HEADER );

   /* Write the bitmap data. */

   o->bmp_no_file_header = 1;

   bmp_file_sz = bmp_write( &(buf_ptr[bmp_data_offset]),
      buf_sz - bmp_data_offset, grid, o );

   /* Write the ICO headers. */

   dir_header->reserved = 0;
   dir_header->type = ICO_TYPE_ICON;
   dir_header->count = 1;

   dir_entry_header->ico_w = 16;
   dir_entry_header->ico_h = 32;
   dir_entry_header->colors = bmp_colors_count( o->bpp );
   dir_entry_header->reserved = 0;
   dir_entry_header->ico_planes_cur_hotspot_h = 1;
   dir_entry_header->ico_bpp_cur_hotspot_v = o->bpp;
   dir_entry_header->bmp_offset = bmp_data_offset;

   bmp_header = &(buf_ptr[
      sizeof( struct ICO_DIR_HEADER ) +
      sizeof( struct ICO_DIR_ENTRY_HEADER )]);
   bmp_mask_sz =
      bmp_header->bitmap_w *
      bmp_header->bitmap_h /
      (8 / bmp_header->bpp);
   bmp_mask_offset = bmp_data_offset + bmp_file_sz;
   assert( bmp_mask_offset + bmp_mask_sz <= buf_sz );

   for( i = bmp_file_sz ; bmp_mask_offset + bmp_mask_sz > i ; i++ ) {
      buf_ptr[i] = 0xff;
   }

   dir_entry_header->data_sz = bmp_file_sz;

   return bmp_data_offset + bmp_file_sz;
}

