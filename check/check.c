
#include <check.h>

#define main_add_test_proto( suite_name ) \
   Suite* suite_name ## _suite();

#define main_add_test( suite_name ) \
   for( i = 0 ; argc > i ; i++ ) { \
      if( \
         1 < argc && \
         0 != strncmp( #suite_name, argv[i], strlen( #suite_name ) ) \
      ) { \
         continue; \
      } \
      Suite* s_ ## suite_name = suite_name ## _suite(); \
      SRunner* sr_ ## suite_name = srunner_create( s_ ## suite_name ); \
      if( nofork ) { \
         srunner_set_fork_status( sr_ ## suite_name, CK_NOFORK ); \
      } \
      srunner_run_all( sr_ ## suite_name, verbose ); \
      number_failed += srunner_ntests_failed( sr_ ## suite_name ); \
      srunner_free( sr_ ## suite_name ); \
      break; \
   }

main_add_test_proto( mobile )
main_add_test_proto( tilemap )
main_add_test_proto( graphics )
main_add_test_proto( data_img )
main_add_test_proto( data_js )
main_add_test_proto( item )
main_add_test_proto( engines )
main_add_test_proto( window )
main_add_test_proto( dio )
main_add_test_proto( drc )

int main( int argc, char* argv[] ) {
   int number_failed = 0,
      i = 0,
      nofork = 0,
      verbose = CK_VERBOSE;

   main_add_test( mobile );
   main_add_test( tilemap );
   main_add_test( graphics );
   main_add_test( data_img );
   main_add_test( data_js );
   main_add_test( item );
   main_add_test( engines );
   main_add_test( window );
   main_add_test( dio );
   main_add_test( drc );

   return( number_failed == 0 ) ? 0 : 1;
}

