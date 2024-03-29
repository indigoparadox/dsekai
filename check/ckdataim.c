
#include <check.h>

#include "../src/dstypes.h"
#include "../tools/convert.h"
#include "../tools/data/cga.h"
#include "../tools/data/bmp.h"
#include "../tools/data/icns.h"

#define CKDATA_IM_C
#include "testdata.h"

void buffer_printf( uint8_t* buffer, int start, int end, int col_break ) {
   int column_idx = 1,
      i = 0;

   for( i = start ; end > i ; i++ ) {
      if( 0 != buffer[i] ) {
         printf( "0x%02x, ", buffer[i] );
      } else {
         printf( "    , " );
      }

      if( 0 == column_idx % col_break ) {
         printf( "\n" );
      }
      column_idx++;
   }

   printf( "\n\n" );
}

START_TEST( check_data_cga_read ) {
   struct CONVERT_GRID* grid = NULL;
   struct CONVERT_OPTIONS options;

   memset( &options, '\0', sizeof( struct CONVERT_OPTIONS ) );
   options.cga_use_header = 1;

   grid = cga_read(
      (const uint8_t*)&gc_test_cga_16_16_4, TEST_CGA_16_16_4_SZ, &options );

   ck_assert_int_eq( gc_test_grid_16_16_4.sz_x, grid->sz_x );
   ck_assert_int_eq( gc_test_grid_16_16_4.sz_y, grid->sz_y );
   ck_assert_int_eq( gc_test_grid_16_16_4.bpp, grid->bpp );
   ck_assert_int_eq( gc_test_grid_16_16_4.data_sz, grid->data_sz );
   ck_assert_int_eq( gc_test_grid_16_16_4_data[_i], grid->data[_i] );

   /* dio_print_grid( grid ); */

   free( grid->data );
   free( grid );
}
END_TEST

START_TEST( check_data_cga_write ) {
   struct CONVERT_OPTIONS options;
   uint8_t buffer[TEST_CGA_16_16_4_SZ];
   struct CGA_HEADER* cga_header = (struct CGA_HEADER*)buffer,
      * test_header = (struct CGA_HEADER*)gc_test_cga_16_16_4;

   memset( &buffer, '\0', TEST_CGA_16_16_4_SZ );
   memset( &options, '\0', sizeof( struct CONVERT_OPTIONS ) );
   options.cga_use_header = 1;
   options.bpp = 2;

   cga_write( 
      (uint8_t*)&buffer, TEST_CGA_16_16_4_SZ, &gc_test_grid_16_16_4, &options );

   ck_assert_int_eq( test_header->version, cga_header->version );
   ck_assert_int_eq( test_header->width, cga_header->width );
   ck_assert_int_eq( test_header->height, cga_header->height );
   ck_assert_int_eq( test_header->bpp, cga_header->bpp );
   ck_assert_int_eq( test_header->plane1_offset, cga_header->plane1_offset );
   ck_assert_int_eq( CGA_HEADER_SZ, cga_header->plane1_offset );
   ck_assert_int_eq( gc_test_cga_16_16_4[_i], buffer[_i] );
}
END_TEST

START_TEST( check_data_icns_read ) {
   struct CONVERT_GRID* grid = NULL;
   struct CONVERT_OPTIONS options;

   memset( &options, '\0', sizeof( struct CONVERT_OPTIONS ) );

   grid = icns_read(
      (const uint8_t*)&gc_test_icns_16_16_2, TEST_CGA_16_16_4_SZ, &options );

   ck_assert_int_eq( gc_test_grid_16_16_4.sz_x, grid->sz_x );
   ck_assert_int_eq( gc_test_grid_16_16_4.sz_y, grid->sz_y );
   ck_assert_int_eq( gc_test_grid_16_16_4.bpp, 2 );
   ck_assert_int_eq( gc_test_grid_16_16_4.data_sz, grid->data_sz );
   ck_assert_int_eq( (gc_test_grid_16_16_4_data[_i] & 0x01), grid->data[_i] );

   /* dio_print_grid( grid ); */

   free( grid->data );
   free( grid );
}
END_TEST

START_TEST( check_data_icns_write_data ) {
   struct CONVERT_OPTIONS options;
   uint8_t buffer[TEST_ICNS_16_16_2_SZ];
   struct ICNS_FILE_HEADER* file_header = (struct ICNS_FILE_HEADER*)buffer,
      * test_header = (struct ICNS_FILE_HEADER*)gc_test_icns_16_16_2;

   memset( &buffer, '\0', TEST_ICNS_16_16_2_SZ );
   memset( &options, '\0', sizeof( struct CONVERT_OPTIONS ) );
   options.bpp = 1;
   options.reverse = 0;

   icns_write( 
      (uint8_t*)&buffer, TEST_ICNS_16_16_2_SZ,
      &gc_test_grid_16_16_4, &options );

   ck_assert_int_eq(
      dio_reverse_endian_32( test_header->file_sz ),
      dio_reverse_endian_32( file_header->file_sz ) );
   ck_assert_int_eq(
      gc_test_icns_16_16_2[ICNS_FILE_HEADER_SZ + ICNS_DATA_HEADER_SZ + _i],
      buffer[ICNS_FILE_HEADER_SZ + ICNS_DATA_HEADER_SZ +_i] );
}
END_TEST

START_TEST( check_data_bmp_write_header ) {
   struct CONVERT_OPTIONS options;
   uint8_t buffer[TEST_BMP_16_16_16_SZ];
   struct BITMAP_FILE_HEADER* file_header = (struct BITMAP_FILE_HEADER*)buffer,
      * test_f_header = (struct BITMAP_FILE_HEADER*)gc_test_bmp_16_16_16;
   struct BITMAP_DATA_HEADER* data_header = (struct BITMAP_DATA_HEADER*)buffer,
      * test_d_header = (struct BITMAP_DATA_HEADER*)&(gc_test_bmp_16_16_16[
         sizeof( struct BITMAP_FILE_HEADER )]);

   memset( &buffer, '\0', TEST_BMP_16_16_16_SZ );
   memset( &options, '\0', sizeof( struct CONVERT_OPTIONS ) );
   options.bpp = 4;

   bmp_write( 
      (uint8_t*)&buffer, TEST_BMP_16_16_16_SZ,
      &gc_test_grid_16_16_4, &options );

}
END_TEST

START_TEST( check_data_bmp_write_data ) {
   struct CONVERT_OPTIONS options;
   uint8_t buffer[TEST_BMP_16_16_16_SZ];
   struct BITMAP_FILE_HEADER* file_header = (struct BITMAP_FILE_HEADER*)buffer,
      * test_f_header = (struct BITMAP_FILE_HEADER*)gc_test_bmp_16_16_16;
   struct BITMAP_DATA_HEADER* data_header = (struct BITMAP_DATA_HEADER*)buffer,
      * test_d_header = (struct BITMAP_DATA_HEADER*)&(gc_test_bmp_16_16_16[
         sizeof( struct BITMAP_FILE_HEADER )]);

   memset( &buffer, '\0', TEST_BMP_16_16_16_SZ );
   memset( &options, '\0', sizeof( struct CONVERT_OPTIONS ) );
   options.bpp = 4;

   bmp_write( 
      (uint8_t*)&buffer, TEST_BMP_16_16_16_SZ,
      &gc_test_grid_16_16_4, &options );

   /* ck_assert_int_eq( test_f_header->file_sz, file_header->file_sz ); */
   ck_assert_int_eq(
      gc_test_bmp_16_16_16[
         sizeof( struct BITMAP_FILE_HEADER ) +
         sizeof( struct BITMAP_DATA_HEADER ) + _i],
      buffer[
         sizeof( struct BITMAP_FILE_HEADER ) +
         sizeof( struct BITMAP_DATA_HEADER ) + _i] );
}
END_TEST

Suite* data_img_suite( void ) {
   Suite* s;
   TCase* tc_bmp,
      *tc_cga,
      *tc_icns;

   s = suite_create( "data_img" );

   /* CGA test case */
   tc_cga = tcase_create( "CGA" );

   tcase_add_loop_test( tc_cga, check_data_cga_read, 0, 256 );
   tcase_add_loop_test( tc_cga, check_data_cga_write, 0, TEST_CGA_16_16_4_SZ );

   suite_add_tcase( s, tc_cga );

   /* ICNS test case */
   tc_icns = tcase_create( "ICNS" );

   tcase_add_loop_test( tc_icns, check_data_icns_read, 0, 256 );
   tcase_add_loop_test(
      tc_icns, check_data_icns_write_data, 0, TEST_ICNS_16_16_2_DATA_SZ );

   suite_add_tcase( s, tc_icns );

   /* BMP test case */
   tc_bmp = tcase_create( "BMP" );

   tcase_add_test( tc_bmp, check_data_bmp_write_header );
   tcase_add_loop_test(
      tc_bmp, check_data_bmp_write_data, 
      sizeof( struct BITMAP_FILE_HEADER ) +
         sizeof( struct BITMAP_DATA_HEADER ),
      sizeof( struct BITMAP_FILE_HEADER ) +
         sizeof( struct BITMAP_DATA_HEADER ) + ((16 * 16) / 2) );

   suite_add_tcase( s, tc_bmp );

   return s;
}

