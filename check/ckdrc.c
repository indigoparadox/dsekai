
#include <check.h>

#include "../src/drc.h"
#include "../tools/data/drcwrite.h"

#define DRC_CKBUFFER_SZ 10240

uint8_t g_buffer_r[DRC_CKBUFFER_SZ];
uint8_t g_buffer_w[DRC_CKBUFFER_SZ];
struct DIO_STREAM g_drc_stream_r;
struct DIO_STREAM g_drc_stream_w;

static void print_buffers() {
   int i = 0;

   printf( "Header: " );
   for( i = 0 ; 140 > i ; i++ ) {
      if( sizeof( struct DRC_HEADER ) == i ) {
         printf( "\nTOC: " );
      } else if(
         sizeof( struct DRC_HEADER ) + sizeof( struct DRC_TOC_E ) == i
      ) {
         printf( "\nData: " );
      }
      printf( "%02x ", g_buffer_r[i] );
   }
   printf( "\n\n" );

   printf( "Header: " );
   for( i = 0 ; 140 > i ; i++ ) {
      if( sizeof( struct DRC_HEADER ) == i ) {
         printf( "\nTOC: " );
      } else if(
         sizeof( struct DRC_HEADER ) + sizeof( struct DRC_TOC_E ) == i
      ) {
         printf( "\nData: " );
      }
      printf( "%02x ", g_buffer_w[i] );
   }
   printf( "\n" );
}

static void drc_setup() {
   /* dio_remove_file( "/tmp/drc_check.drc" );
   dio_open_stream_file( "/tmp/drc_check.drc", ); */
   memset( &g_buffer_w, '\0', DRC_CKBUFFER_SZ );
   memset( &g_buffer_r, '\0', DRC_CKBUFFER_SZ );
   dio_open_stream_buffer( g_buffer_r, DRC_CKBUFFER_SZ, &g_drc_stream_r );
   dio_open_stream_buffer( g_buffer_w, DRC_CKBUFFER_SZ, &g_drc_stream_w );
   ck_assert_int_eq( 1, dio_type_stream( &g_drc_stream_r ) );
   ck_assert_int_eq( 1, dio_type_stream( &g_drc_stream_w ) );
}

static void drc_teardown() {
   dio_close_stream( &g_drc_stream_r );
   dio_close_stream( &g_drc_stream_w );
}

START_TEST( check_drc_create ) {
   struct DRC_HEADER* header = (struct DRC_HEADER*)&g_buffer_w;
   drc_create( &g_drc_stream_w );
   ck_assert_int_eq(
      dio_tell_stream( &g_drc_stream_w ), sizeof( struct DRC_HEADER ) );
   ck_assert_int_eq( header->version, 1 );
   ck_assert_int_eq( header->filesize, sizeof( struct DRC_HEADER ) );
   ck_assert_int_eq( header->toc_start, sizeof( struct DRC_HEADER ) );
   ck_assert_int_eq( header->toc_entries, 0 );
   ck_assert_int_eq( header->first_entry_start, sizeof( struct DRC_HEADER ) );

}
END_TEST

START_TEST( check_drc_add ) {
   int i = 0,
      ids[] = { 5001, 5002 };
   union DRC_TYPE toc_type = DRC_BITMAP_TYPE,
      header_type = DRC_ARCHIVE_TYPE;
   char test_string_1[] = "This is a test string.",
      test_string_2[] = "Another!";
   char test_names[10][10] = {
      "test1.bmp",
      "test2.bmp"
   };
   struct DRC_HEADER* header = (struct DRC_HEADER*)&g_buffer_r;
   struct DRC_TOC_E* toc_e[] = {
         (struct DRC_HEADER*)&(g_buffer_r[sizeof( struct DRC_HEADER )]),
         (struct DRC_HEADER*)&(g_buffer_r[sizeof( struct DRC_HEADER ) +
            sizeof( struct DRC_TOC_E )]) };

   drc_create( &g_drc_stream_r );

   /* Add first resource. */

   drc_add_resource(
      &g_drc_stream_r, &g_drc_stream_w, toc_type, ids[0], test_names[0], 8,
      test_string_1, 23 );

   /* print_buffers(); */

   memcpy( g_buffer_r, g_buffer_w, DRC_CKBUFFER_SZ );
   memset( g_buffer_w, '\0', DRC_CKBUFFER_SZ );

   ck_assert_int_eq( header->type.u32, header_type.u32 );
   ck_assert_int_eq( header->toc_entries, 1 );
   ck_assert_int_eq( toc_e[0]->type.u32, toc_type.u32 );
   ck_assert_int_eq( toc_e[0]->id, ids[0] );
   ck_assert_int_eq( toc_e[0]->data_start,
      sizeof( struct DRC_HEADER ) + sizeof( struct DRC_TOC_E ) );
   ck_assert_int_eq(
      sizeof( struct DRC_HEADER ),
      dio_tell_stream( &g_drc_stream_r ) );
   ck_assert_int_eq(
      sizeof( struct DRC_HEADER ) + sizeof( struct DRC_TOC_E ) + 23,
      dio_tell_stream( &g_drc_stream_w ) );
   ck_assert_str_eq( toc_e[0]->name, "test1.bmp" );
   ck_assert_str_eq( &(g_buffer_r[toc_e[0]->data_start]), test_string_1 );

   /* Add second resource. */

   drc_add_resource(
      &g_drc_stream_r, &g_drc_stream_w, toc_type, ids[1], "test2.bmp", 8,
      test_string_2, 9 );

   memcpy( g_buffer_r, g_buffer_w, DRC_CKBUFFER_SZ );
   memset( g_buffer_w, '\0', DRC_CKBUFFER_SZ );

   for( i = 0 ; 2 > i ; i++ ) {
      ck_assert_int_eq( header->type.u32, header_type.u32 );
      ck_assert_int_eq( header->toc_entries, 2 );
      ck_assert_int_eq( toc_e[i]->type.u32, toc_type.u32 );
      ck_assert_int_eq( toc_e[i]->id, ids[i] );
      ck_assert_str_eq( toc_e[i]->name, test_names[i] );
      ck_assert_str_eq( toc_e[i]->name, test_names[i] );
   }

   ck_assert_int_eq( toc_e[0]->data_start,
      sizeof( struct DRC_HEADER ) +
      (2 * sizeof( struct DRC_TOC_E ))
   );
   ck_assert_int_eq( toc_e[1]->data_start,
      sizeof( struct DRC_HEADER ) +
      (2 * sizeof( struct DRC_TOC_E )) +
      23 /* Size of first data. */
   );
   ck_assert_str_eq( &(g_buffer_r[toc_e[0]->data_start]), test_string_1 );
   ck_assert_str_eq( &(g_buffer_r[toc_e[1]->data_start]), test_string_2 );
   ck_assert_int_eq(
      /* Check stream_w, since that's the one that did the writing. */
      dio_tell_stream( &g_drc_stream_w ),
      sizeof( struct DRC_HEADER ) +
         (2 * sizeof( struct DRC_TOC_E )) +
         23 +
         9
   );

   /* ck_assert_int_eq(
      sizeof( struct DRC_HEADER ) + sizeof( struct DRC_TOC_E ) + 22,
      dio_tell_stream( &g_drc_stream_w ) ); */

   /* print_buffers(); */

}
END_TEST

START_TEST( check_drc_list ) {
   int i = 0,
      toc_sz = 0;
   union DRC_TYPE toc_type = DRC_BITMAP_TYPE,
      header_type = DRC_ARCHIVE_TYPE;
   char test_string_1[] = "This is a test string.",
      test_string_2[] = "Another tstrng.",
      test_string_3[] = "ZYXQRS";
   struct DRC_HEADER* header = (struct DRC_HEADER*)&g_buffer_r;
   struct DRC_TOC_E* toc_e =
         (struct DRC_TOC_E*)&(g_buffer_r[sizeof( struct DRC_HEADER )]),
      * toc_l = NULL;
   MEMORY_HANDLE handle = NULL;
   int32_t filesize_test = 0,
      test_1_start = 0,
      test_2_start = 0;

   drc_create( &g_drc_stream_r );

   /* Add first test. */

   drc_add_resource(
      &g_drc_stream_r, &g_drc_stream_w, toc_type, 5001, "test1.bmp", 9,
      test_string_1, 22 );

   dio_close_stream( &g_drc_stream_w );
   dio_close_stream( &g_drc_stream_r );
   dio_open_stream_buffer( g_buffer_w, DRC_CKBUFFER_SZ, &g_drc_stream_w );
   dio_open_stream_buffer( g_buffer_r, DRC_CKBUFFER_SZ, &g_drc_stream_r );

   memcpy( g_buffer_r, g_buffer_w, DRC_CKBUFFER_SZ );

   toc_sz = drc_list_resources( &g_drc_stream_r, &handle, 0 );

   filesize_test = sizeof( struct DRC_HEADER ) +
      (1 * sizeof( struct DRC_TOC_E )) +
      22;

   test_1_start = sizeof( struct DRC_HEADER ) +
      (1 * sizeof( struct DRC_TOC_E ));

   toc_l = memory_lock( handle );

   ck_assert_int_eq( toc_sz, 1 );
   ck_assert_int_eq( filesize_test, header->filesize );
   ck_assert_int_eq( test_1_start, toc_l[0].data_start );

   toc_l = memory_unlock( handle );

   memory_free( handle );
   handle = NULL;

   /* Add second test. */

   drc_add_resource(
      &g_drc_stream_r, &g_drc_stream_w, toc_type, 5002, "test2.bmp", 9,
      test_string_2, 15 );

   dio_close_stream( &g_drc_stream_w );
   dio_close_stream( &g_drc_stream_r );
   dio_open_stream_buffer( g_buffer_w, DRC_CKBUFFER_SZ, &g_drc_stream_w );
   dio_open_stream_buffer( g_buffer_r, DRC_CKBUFFER_SZ, &g_drc_stream_r );

   memcpy( g_buffer_r, g_buffer_w, DRC_CKBUFFER_SZ );

   toc_sz = drc_list_resources( &g_drc_stream_r, &handle, 0 );

   filesize_test = sizeof( struct DRC_HEADER ) +
      (2 * sizeof( struct DRC_TOC_E )) +
      22 + 15;

   test_1_start = sizeof( struct DRC_HEADER ) +
      (2 * sizeof( struct DRC_TOC_E ));

   test_2_start = sizeof( struct DRC_HEADER ) +
      (2 * sizeof( struct DRC_TOC_E )) +
      22;

   toc_l = memory_lock( handle );

   ck_assert_int_eq( toc_sz, 2 );
   ck_assert_int_eq( filesize_test, header->filesize );
   ck_assert_int_eq( 22, toc_l[0].data_sz );
   ck_assert_int_eq( test_1_start, toc_l[0].data_start );
   ck_assert_int_eq( 15, toc_l[1].data_sz );
   ck_assert_int_eq( test_2_start, toc_l[1].data_start );

   toc_l = memory_unlock( handle );
   
   memory_free( handle );
   handle = NULL;

   /* Add third test. */

   drc_add_resource(
      &g_drc_stream_r, &g_drc_stream_w, toc_type, 5003, "test3.bmp", 9,
      test_string_3, 6 );

   dio_close_stream( &g_drc_stream_w );
   dio_close_stream( &g_drc_stream_r );
   dio_open_stream_buffer( g_buffer_w, DRC_CKBUFFER_SZ, &g_drc_stream_w );
   dio_open_stream_buffer( g_buffer_r, DRC_CKBUFFER_SZ, &g_drc_stream_r );

   memcpy( g_buffer_r, g_buffer_w, DRC_CKBUFFER_SZ );

   toc_sz = drc_list_resources( &g_drc_stream_r, &handle, 0 );

   toc_l = memory_lock( handle );

   ck_assert_int_eq( toc_sz, 3 );
   ck_assert_int_eq( toc_l[0].id, toc_e->id );
   ck_assert_int_eq( toc_l[0].type.u32, toc_e->type.u32 );
   ck_assert_int_eq( toc_l[1].id, 5002 );
   ck_assert_int_eq( toc_l[1].type.u32, toc_e->type.u32 );
   ck_assert_int_eq( toc_l[2].id, 5003 );
   ck_assert_int_eq( toc_l[2].type.u32, toc_e->type.u32 );

   filesize_test = sizeof( struct DRC_HEADER ) +
      (3 * sizeof( struct DRC_TOC_E )) +
      22 + 15 + 6;

   test_1_start = sizeof( struct DRC_HEADER ) +
      (3 * sizeof( struct DRC_TOC_E ));

   test_2_start = sizeof( struct DRC_HEADER ) +
      (3 * sizeof( struct DRC_TOC_E )) +
      22;

   ck_assert_int_eq( test_1_start, toc_l[0].data_start );
   ck_assert_int_eq( test_2_start, toc_l[1].data_start );
   ck_assert_int_eq( filesize_test, header->filesize );

   toc_l = memory_unlock( handle );

   memory_free( handle );
   handle = NULL;
      
}
END_TEST

START_TEST( check_drc_get ) {
   int i = 0,
      toc_sz = 0;
   int32_t read = 0;
   union DRC_TYPE toc_type = DRC_BITMAP_TYPE,
      header_type = DRC_ARCHIVE_TYPE;
   char test_string_1[] = "This is a test string.",
      test_string_2[] = "Another tstrng.",
      test_data_buffer[1024],
      test_name_buffer[1024],
      reader[24];
   struct DRC_HEADER* header = (struct DRC_HEADER*)&g_buffer_r;
   struct DRC_TOC_E* toc_e =
         (struct DRC_TOC_E*)&(g_buffer_r[sizeof( struct DRC_HEADER )]),
      * toc_l = NULL;

   drc_create( &g_drc_stream_r );

   drc_add_resource(
      &g_drc_stream_r, &g_drc_stream_w, toc_type, 5001, "test1.bmp", 9,
      test_string_1, 22 );

   memcpy( g_buffer_r, g_buffer_w, DRC_CKBUFFER_SZ );
   memset( g_buffer_w, '\0', DRC_CKBUFFER_SZ );

   drc_add_resource(
      &g_drc_stream_r, &g_drc_stream_w, toc_type, 5002, "test2.bmp", 9,
      test_string_2, 15 );

   memcpy( g_buffer_r, g_buffer_w, DRC_CKBUFFER_SZ );
   memset( g_buffer_w, '\0', DRC_CKBUFFER_SZ );

   for( i = 0 ; 20 > i ; i++ ) {
      memset( test_data_buffer, '\0', 1024 );
      memset( test_name_buffer, '\0', 1024 );
      snprintf( test_data_buffer, 1024, "String %02d here.", i );
      snprintf( test_name_buffer, 1024, "test%d.bmp", i );
      printf( "adding %s\n", test_name_buffer );

      drc_add_resource(
         &g_drc_stream_r, &g_drc_stream_w, toc_type,
         5010 + i, test_name_buffer, 9,
         test_data_buffer, 15 );

      memcpy( g_buffer_r, g_buffer_w, DRC_CKBUFFER_SZ );
      memset( g_buffer_w, '\0', DRC_CKBUFFER_SZ );
   }

   dio_close_stream( &g_drc_stream_r );
   dio_open_stream_buffer( g_buffer_r, DRC_CKBUFFER_SZ, &g_drc_stream_r );

   read = drc_get_resource_sz( &g_drc_stream_r, toc_type, 5001 );

   ck_assert_int_eq( read, 22 );
      
   read = drc_get_resource( &g_drc_stream_r, toc_type, 5001, reader, 24 );

   ck_assert_int_eq( read, 22 );
   ck_assert_str_eq( reader, test_string_1 );
}
END_TEST

Suite* drc_suite( void ) {
   Suite* s;
   TCase* tc_core;

   s = suite_create( "drc" );

   /* Core test case */
   tc_core = tcase_create( "Core" );
   tcase_add_checked_fixture( tc_core, drc_setup, drc_teardown );

   tcase_add_test( tc_core, check_drc_create ); 
   tcase_add_test( tc_core, check_drc_add ); 
   tcase_add_test( tc_core, check_drc_list ); 
   tcase_add_test( tc_core, check_drc_get ); 

   suite_add_tcase( s, tc_core );

   return s;
}

