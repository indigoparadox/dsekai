
#include <check.h>

#include "../src/memory.h"

START_TEST( check_memory_alloc ) {
   struct FAKE_MEMORY_HANDLE* handle = NULL;

   handle = memory_alloc( 1, 1024 );
   
   ck_assert_ptr_ne( handle, NULL );

   memory_free( handle );
}
END_TEST

START_TEST( check_memory_write ) {
   struct FAKE_MEMORY_HANDLE* handle = NULL;
   char* ptr = NULL;
   const char test_str[] = "The quick brown fox jumped over the lazy dog.";

   handle = memory_alloc( 1, 1024 );

   ptr = memory_lock( handle );
   memcpy( ptr, test_str, strlen( test_str ) + 1 );
   memory_unlock( handle );

   ck_assert_str_eq( test_str, handle->ptr );

}
END_TEST

START_TEST( check_memory_lock ) {
   struct FAKE_MEMORY_HANDLE* handle1 = NULL,
      * handle2 = NULL;
   uint8_t* buffer1 = NULL,
      * buffer2 = NULL;

   handle1 = memory_alloc( 1, 10 );
   handle2 = memory_alloc( 1, 20 );

   ck_assert_int_eq( 0, handle1->locks );
   ck_assert_int_eq( 0, handle2->locks );
   ck_assert_int_eq( 10, memory_sz( handle1 ) );
   ck_assert_int_eq( 20, memory_sz( handle2 ) );

   buffer1 = memory_lock( handle1 );
   buffer2 = memory_lock( handle2 );

   ck_assert_int_eq( 1, handle1->locks );
   ck_assert_int_eq( 1, handle2->locks );
   ck_assert_int_eq( 10, memory_sz( handle1 ) );
   ck_assert_int_eq( 20, memory_sz( handle2 ) );
   ck_assert_ptr_eq( buffer1, handle1->ptr );
   ck_assert_ptr_eq( buffer2, handle2->ptr );

   buffer1 = memory_unlock( handle1 );
   buffer2 = memory_unlock( handle2 );

   ck_assert_int_eq( 10, memory_sz( handle1 ) );
   ck_assert_int_eq( 20, memory_sz( handle2 ) );

   ck_assert_int_eq( 0, handle1->locks );
   ck_assert_int_eq( 0, handle2->locks );

   memory_free( handle1 );
   memory_free( handle2 );

}
END_TEST

Suite* memory_suite( void ) {
   Suite* s;
   TCase* tc_core;

   s = suite_create( "memory" );

   tc_core = tcase_create( "Core" );

   tcase_add_test( tc_core, check_memory_alloc );
   tcase_add_test( tc_core, check_memory_write );
   tcase_add_test( tc_core, check_memory_lock );

   suite_add_tcase( s, tc_core );

   return s;
}

