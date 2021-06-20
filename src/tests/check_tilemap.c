
#include <check.h>

#define MAIN_C
#include "../data.h"

Suite* tilemap_suite( void ) {
   Suite* s;
   TCase* tc_core;

   s = suite_create( "tilemap" );

   /* Core test case */
   tc_core = tcase_create( "Core" );

   suite_add_tcase( s, tc_core );

   return s;
}

