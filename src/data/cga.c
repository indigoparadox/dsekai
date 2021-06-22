
#include "cga.h"

int cga_write( const char* path, const struct CONVERT_GRID* grid ) {
   int retval = 0;

   return retval;
}

#define CGA_BPP 2

struct CONVERT_GRID* cga_read( const char* path, int sz_x, int sz_y ) {
   FILE* cga_file = NULL;
   size_t read = 0,
      cga_file_sz = 0,
      byte_idx = 0,
      bit_idx = 0,
      grid_idx_odd = 0,
      grid_idx_even = 0,
      i = 0,
      y = 0,
      x = 0;
   uint8_t* raw_cga_data = NULL;
   uint8_t byte_buffer_odd = 0,
      byte_buffer_even = 0;
   struct CONVERT_GRID* grid = NULL;

   cga_file = fopen( path, "rb" );
   assert( NULL != cga_file );

   fseek( cga_file, 0, SEEK_END );
   cga_file_sz = ftell( cga_file );

   fseek( cga_file, 0, SEEK_SET );

   raw_cga_data = calloc( 1, cga_file_sz + 1 );
   assert( NULL != raw_cga_data );

   read = fread( raw_cga_data, 1, cga_file_sz, cga_file );
   assert( read == cga_file_sz );

   convert_printf( "read CGA file: %lu bytes\n", cga_file_sz );

   grid = calloc( 1, sizeof( struct CONVERT_GRID ) );
   assert( NULL != grid );
   grid->data_sz = sz_x * sz_y;
   grid->data = calloc( 1, grid->data_sz );
   assert( NULL != grid->data );

   grid->sz_x = sz_x;
   grid->sz_y = sz_y;
   grid->bpp = 2;

   y = 0;
   printf( "len: %d\n\n", ((sz_x * sz_y) / 8) );
   while( 8000 > byte_idx ) {

      byte_buffer_odd = raw_cga_data[byte_idx];
      byte_buffer_even = raw_cga_data[byte_idx + 8192];

      grid_idx_even = (y * sz_x) + x;
      grid_idx_odd = grid_idx_even + sz_x;
      assert( x + 3 < sz_x );
         
      grid->data[grid_idx_odd] |= byte_buffer_odd & 0x03;
      grid->data[grid_idx_odd + 1] |= byte_buffer_odd & 0x0c;
      grid->data[grid_idx_odd + 1] >>= 2;
      grid->data[grid_idx_odd + 2] |= byte_buffer_odd & 0x30;
      grid->data[grid_idx_odd + 2] >>= 4;
      grid->data[grid_idx_odd + 3] |= byte_buffer_odd & 0xc0;
      grid->data[grid_idx_odd + 3] >>= 6;
      grid->data[grid_idx_even] |= byte_buffer_even & 0x03;
      grid->data[grid_idx_even + 1] |= byte_buffer_even & 0x0c;
      grid->data[grid_idx_even + 1] >>= 2;
      grid->data[grid_idx_even + 2] |= byte_buffer_even & 0x30;
      grid->data[grid_idx_even + 2] >>= 4;
      grid->data[grid_idx_even + 3] |= byte_buffer_even & 0xc0;
      grid->data[grid_idx_even + 3] >>= 6;
      byte_idx++;
      x += 4;

      if( x >= sz_x ) {
         /* Move to the next row. */
         y += 2;
         x = 0;
      }
   }

   fclose( cga_file );

   free( raw_cga_data );

   return grid;
}

