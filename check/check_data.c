
#include <check.h>

#include "../tools/convert.h"
#include "../tools/data/cga.h"
#include "../tools/data/bmp.h"
#include "../tools/data/icns.h"
#include "../tools/data/json.h"

#define TEST_CGA_16_16_4_SZ (CGA_HEADER_SZ + (2 * 32))

static const uint8_t gc_test_cga_16_16_4[TEST_CGA_16_16_4_SZ] = {
   'C', 'G',
   0x02, 0x00,                /* Version      1 */
   0x10, 0x00,                /* Width:      16 */
   0x10, 0x00,                /* Height:     16 */
   0x02, 0x00,                /* BPP:         2 */
   CGA_HEADER_SZ, 0x00,       /* Plane1 Ofs: 20 */
   0x20, 0x00,                /* Plane1 Siz: 32 */
   CGA_HEADER_SZ + 32, 0x00,  /* Plane2 Ofs: 52 */
   0x20, 0x00,                /* Plane2 Siz: 32 */
   0x01, 0x00,                /* Palette:     1 */
   0x00, 0x00,                /* Big Endian     */
   0x00, 0x00,                /* Reserved       */
   0x00, 0x00,                /* Reserved       */
   0x00, 0x00,                /* Reserved       */
   0x00, 0x00,                /* Reserved       */

   /* End Header */
   
   /* Plane 1 */
   0xff, 0xff, 0xff, 0xff, /* 0 Offset: 20 */
   0xf0, 0x00, 0x00, 0x0f, /* 1 */
   0xf0, 0x00, 0x00, 0x0f, /* 2 */
   0xf0, 0x00, 0x00, 0x0f, /* 3 */
   0xf0, 0x0f, 0xf0, 0x0f, /* 4 */
   0xf0, 0x00, 0x00, 0x0f, /* 5 */
   0xf0, 0x00, 0x00, 0x0f, /* 6 */
   0xff, 0xff, 0xff, 0xff, /* 7 */

   /* Plane 2 */
   0xff, 0xff, 0xff, 0xff, /* 0 Offset: 52 */
   0xf0, 0x00, 0x00, 0x0f, /* 1 */ 
   0xf0, 0x00, 0x00, 0x0f, /* 2 */
   0xf0, 0x0f, 0xf0, 0x0f, /* 3 */
   0xf0, 0x00, 0x00, 0x0f, /* 4 */
   0xf0, 0x00, 0x00, 0x0f, /* 5 */
   0xf0, 0x00, 0x00, 0x0f, /* 6 */
   0xff, 0xff, 0xff, 0xff, /* 7 */
};

static const uint8_t gc_test_cga_16_16_4_le[TEST_CGA_16_16_4_SZ] = {
   'C', 'G',
   0x02, 0x00,                /* Version      1 */
   0x10, 0x00,                /* Width:      16 */
   0x10, 0x00,                /* Height:     16 */
   0x02, 0x00,                /* BPP:         2 */
   CGA_HEADER_SZ, 0x00,       /* Plane1 Ofs: 20 */
   0x20, 0x00,                /* Plane1 Siz: 32 */
   CGA_HEADER_SZ + 32, 0x00,  /* Plane2 Ofs: 52 */
   0x20, 0x00,                /* Plane2 Siz: 32 */
   0x01, 0x00,                /* Palette:     1 */
   0x01, 0x00,                /* Little Endian  */
   0x00, 0x00,                /* Reserved       */
   0x00, 0x00,                /* Reserved       */
   0x00, 0x00,                /* Reserved       */
   0x00, 0x00,                /* Reserved       */

   /* End Header */
   
   /* Plane 1 */
   0xff, 0xff, 0xff, 0xff, /* 0 Offset: 20 */
   0x00, 0x0f, 0xf0, 0x00, /* 1 */
   0x00, 0x0f, 0xf0, 0x00, /* 2 */
   0x00, 0x0f, 0xf0, 0x00, /* 3 */
   0xf0, 0x0f, 0xf0, 0x0f, /* 4 */
   0x00, 0x0f, 0xf0, 0x00, /* 5 */
   0x00, 0x0f, 0xf0, 0x00, /* 6 */
   0xff, 0xff, 0xff, 0xff, /* 7 */

   /* Plane 2 */
   0xff, 0xff, 0xff, 0xff, /* 0 Offset: 52 */
   0x00, 0x0f, 0xf0, 0x00, /* 1 */ 
   0x00, 0x0f, 0xf0, 0x00, /* 2 */
   0xf0, 0x0f, 0xf0, 0x0f, /* 3 */
   0x00, 0x0f, 0xf0, 0x00, /* 4 */
   0x00, 0x0f, 0xf0, 0x00, /* 5 */
   0x00, 0x0f, 0xf0, 0x00, /* 6 */
   0xff, 0xff, 0xff, 0xff, /* 7 */
};

#define TEST_ICNS_16_16_2_DATA_SZ (((16 * 16) / 8) * 2)
#define TEST_ICNS_16_16_2_SZ \
   (ICNS_FILE_HEADER_SZ + ICNS_DATA_HEADER_SZ + TEST_ICNS_16_16_2_DATA_SZ)

static const uint8_t gc_test_icns_16_16_2[TEST_ICNS_16_16_2_SZ] = {

   0x69, 0x63, 0x6e, 0x73,
   0x00, 0x00, 0x00, TEST_ICNS_16_16_2_SZ,

   /* Data Header */

   'i', 'c', 's', '#',
   0x00, 0x00, 0x00, 0x48,

   /* Data */

   0xff, 0xff,
   0xff, 0xff,
   0xc0, 0x03,
   0xc0, 0x03,
   0xc0, 0x03,
   0xc0, 0x03,
   0xc0, 0x03,
   0xc3, 0xc3,
   0xc3, 0xc3,
   0xc0, 0x03,
   0xc0, 0x03,
   0xc0, 0x03,
   0xc0, 0x03,
   0xc0, 0x03,
   0xff, 0xff,
   0xff, 0xff,

   /* Mask */

   0xff, 0xff,
   0xff, 0xff,
   0xc0, 0x03,
   0xc0, 0x03,
   0xc0, 0x03,
   0xc0, 0x03,
   0xc0, 0x03,
   0xc3, 0xc3,
   0xc3, 0xc3,
   0xc0, 0x03,
   0xc0, 0x03,
   0xc0, 0x03,
   0xc0, 0x03,
   0xc0, 0x03,
   0xff, 0xff,
   0xff, 0xff,
};

static const uint8_t gc_test_grid_16_16_4_data[256] = {
   0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, /* 3333333333333333 */
   0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, /* 00 Even 00 */

   0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, /* 3333333333333333 */
   0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, /* 01 Odd  00 */

   0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 3300000000000033 */
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03, /* 02 Even 01 */

   0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 3300000000000033 */
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03, /* 03 Odd  01 */

   0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 3300000000000033 */
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03, /* 04 Even 02 */

   0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 3300000000000033 */
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03, /* 05 Odd  02 */

   0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 3300000000000033 */
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03, /* 06 Even 03 */

   0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03, /* 3300003333000033 */
   0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03, /* 07 Odd  03 */

   0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03, /* 3300003333000033 */
   0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03, /* 08 Even 04 */

   0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 3300000000000033 */
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03, /* 09 Odd  04 */

   0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 3300000000000033 */
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03, /* 10 Even 05 */

   0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 3300000000000033 */
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03, /* 11 Odd  05 */

   0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 3300000000000033 */
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03, /* 12 Even 06 */

   0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 3300000000000033 */
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03, /* 13 Odd  06 */

   0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, /* 3333333333333333 */
   0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, /* 14 Even 07 */

   0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, /* 3333333333333333 */
   0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, /* 15 Odd  07 */
};

static const struct CONVERT_GRID gc_test_grid_16_16_4 = {
   16,
   16,
   2,
   256,
   (uint8_t*)&gc_test_grid_16_16_4_data
};

static const char gc_test_json[] = {
   "{\"objects_sz\": 3, \"objects\": [{\"name\":\"foo\"},{\"name\":\"bar\"},{\"name\":\"baz\",\"extra\":12}]}"
};

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

START_TEST( check_data_icns_write ) {
   struct CONVERT_OPTIONS options;
   uint8_t buffer[TEST_ICNS_16_16_2_SZ];
   struct ICNS_FILE_HEADER* file_header = (struct ICNS_FILE_HEADER*)buffer,
      * test_header = (struct ICNS_FILE_HEADER*)gc_test_icns_16_16_2;

   memset( &buffer, '\0', TEST_ICNS_16_16_2_SZ );
   memset( &options, '\0', sizeof( struct CONVERT_OPTIONS ) );
   options.bpp = 1;

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

START_TEST( check_data_json_parse ) {
   json_parse_buffer( &gc_test_json, 0 );
}
END_TEST

Suite* data_suite( void ) {
   Suite* s;
   TCase* tc_core;

   s = suite_create( "data" );

   /* Core test case */
   tc_core = tcase_create( "Core" );

   tcase_add_loop_test( tc_core, check_data_cga_read, 0, 256 );
   tcase_add_loop_test( tc_core, check_data_cga_write, 0, TEST_CGA_16_16_4_SZ );
   tcase_add_loop_test( tc_core, check_data_icns_read, 0, 256 );
   tcase_add_loop_test(
      tc_core, check_data_icns_write, 0, TEST_ICNS_16_16_2_DATA_SZ );
   tcase_add_test( tc_core, check_data_json_parse );

   suite_add_tcase( s, tc_core );

   return s;
}

