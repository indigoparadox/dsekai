
#include <check.h>

#include "../src/dstypes.h"

#define CKDATA_JS_C
#include "testdata.h"

#define JSON_TEST_TOKENS_MAX 50

static int g_tokens_parsed = 0;
static jsmn_parser g_parser;
static jsmntok_t g_tokens[JSON_TEST_TOKENS_MAX];

static void data_js_setup() {
   memset( &g_parser, '\0', sizeof( jsmn_parser ) );
   memset( &g_tokens, '\0', JSON_TEST_TOKENS_MAX * sizeof( jsmntok_t ) );
   jsmn_init( &g_parser );
   g_tokens_parsed = jsmn_parse(
      &g_parser, gc_test_json, strlen( gc_test_json ),
      g_tokens, JSON_TEST_TOKENS_MAX );
   ck_assert_int_gt( g_tokens_parsed, 0 );
}

static void data_js_teardown() {

}

START_TEST( check_data_json_parse_obj_first ) {
   int id = 0;

   id = json_token_id_from_path(
      "/objects/0/name", 15, &(g_tokens[0]), g_tokens_parsed, gc_test_json );

   ck_assert_int_eq( id, 7 );
}
END_TEST

START_TEST( check_data_json_parse_multi_obj ) {
   int id = 0;

   id = json_token_id_from_path(
      "/objects_sz", 11, &(g_tokens[0]), g_tokens_parsed, gc_test_json );

   ck_assert_int_eq( id, 2 );
}
END_TEST

START_TEST( check_data_json_parse_list ) {
   int id = 0,
      val = 0;

   id = json_token_id_from_path(
      "/items/3", 8, &(g_tokens[0]), g_tokens_parsed, gc_test_json );

   ck_assert_int_eq( id, 37 );

   val = dio_atoi( &(gc_test_json[g_tokens[id].start]), 10 );
   ck_assert_int_eq( val, 44 );
}
END_TEST

START_TEST( check_data_json_parse_through_combi ) {
   int id = 0;

   id = json_token_id_from_path(
      "/objects/0/extra/2", 18, &(g_tokens[0]), g_tokens_parsed, gc_test_json );

   ck_assert_int_eq( id, 16 );
   ck_assert_int_eq( gc_test_json[g_tokens[id].start], 'z' );
}
END_TEST

START_TEST( check_data_json_parse_tileset_path ) {
   int id = 0;

   id = json_token_id_from_path(
      "/tilesets/0/source", 18, &(g_tokens[0]), g_tokens_parsed, gc_test_json );

   ck_assert_int_eq( id, 46 );
   ck_assert_int_eq( gc_test_json[g_tokens[id].start], 'f' );
}
END_TEST

START_TEST( check_data_json_parse_attribs ) {
   int id = 0;

   id = json_token_id_from_path(
      "/objects/[name=fii]/extra/0", 27,
      &(g_tokens[0]), g_tokens_parsed, gc_test_json );

   ck_assert_int_eq( id, 26 );
   ck_assert_int_eq( gc_test_json[g_tokens[id].start], 'h' );
   ck_assert_int_eq( gc_test_json[g_tokens[id].start + 1], 'i' );
   ck_assert_int_eq( gc_test_json[g_tokens[id].start + 2], 'z' );
   ck_assert_int_eq( gc_test_json[g_tokens[id].start + 3], 'z' );
}
END_TEST

Suite* data_js_suite( void ) {
   Suite* s;
   TCase* tc_core;

   s = suite_create( "data_js" );

   /* Core test case */
   tc_core = tcase_create( "Core" );
   tcase_add_checked_fixture( tc_core, data_js_setup, data_js_teardown );

   tcase_add_test( tc_core, check_data_json_parse_obj_first );
   tcase_add_test( tc_core, check_data_json_parse_multi_obj );
   tcase_add_test( tc_core, check_data_json_parse_list );
   tcase_add_test( tc_core, check_data_json_parse_through_combi );
   tcase_add_test( tc_core, check_data_json_parse_tileset_path );
   tcase_add_test( tc_core, check_data_json_parse_attribs );

   suite_add_tcase( s, tc_core );

   return s;
}

