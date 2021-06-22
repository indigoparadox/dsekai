
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
   while( cga_file_sz / 2 > byte_idx ) {
      convert_printf( "byte_idx %lu, bit_idx %lu, row %lu, col %lu (%lu)\n",
         byte_idx, bit_idx, y, x, (y * sz_x) + x );
      /*if( 0 == bit_idx % 8 ) {
         byte_buffer_odd = raw_cga_data[byte_idx];
         byte_buffer_even = raw_cga_data[byte_idx + 8192];
         //byte_idx++;
         bit_idx = 0;
      }*/

      byte_buffer_odd = raw_cga_data[byte_idx];
      byte_buffer_even = raw_cga_data[byte_idx + 8192];

      /* Read this pixel into the grid. */
      grid_idx_odd = (y * sz_x) + x;
      grid_idx_even = grid_idx_odd + sz_x;
      //assert( grid_idx < (cga_file_sz * 4) );
      printf( "%d\n", x );
         
      //for( i = 0 ; 1 > i ; i += 2 ) {
         //convert_print_binary( byte_buffer );
         //printf( "becomes " );
         //if( 0 == y % 2 ) {
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
         //}
         //}
         //convert_print_binary( grid->data[grid_idx] );
      //bit_idx += grid->bpp *; /* 2 */
      byte_idx++;
      x += 4;
      //}
      //grid->data[grid_idx_odd] >>= (bit_idx - grid->bpp);
      //grid->data[grid_idx_even] >>= (bit_idx - grid->bpp);
      //assert( grid->data[grid_idx] < 4 );

      /* Move to the next pixel. */
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

