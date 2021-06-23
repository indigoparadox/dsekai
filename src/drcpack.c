
#include "data/drc.h"
#include "data/dio.h"
#include "data/preproc.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STATE_INFILE    1
#define STATE_ARCFILE   2
#define STATE_TYPE      3
#define STATE_ID        4

#define COMMAND_CREATE  1
#define COMMAND_ADD     2
#define COMMAND_LIST    3

int main( int argc, char* argv[] ) {
   int i = 0,
      retval = 0,
      state = 0,
      command = 0;
   uint32_t id = 0,
      file_sz = 0;
   char namebuf_in[NAMEBUF_MAX + 1],
      namebuf_arc[NAMEBUF_MAX + 1],
      type_buf[5];
   uint8_t* file_contents = NULL;

   memset( namebuf_in, '\0', NAMEBUF_MAX + 1 );
   memset( namebuf_arc, '\0', NAMEBUF_MAX + 1 );
   memset( type_buf, '\0', 5 );

   for( i = 1 ; argc > i ; i++ ) {
      switch( state ) {
      case STATE_INFILE:
         strncpy( namebuf_in, argv[i], NAMEBUF_MAX );
         state = 0;
         break;

      case STATE_ARCFILE:
         strncpy( namebuf_arc, argv[i], NAMEBUF_MAX );
         state = 0;
         break;

      case STATE_TYPE:
         strncpy( type_buf, argv[i], 4 );
         state = 0;
         break;

      case STATE_ID:
         id = *((uint32_t*)argv[i]);
         state = 0;
         break;

      default:
         if( 0 == strncmp( argv[i], "-if", 3 ) ) {
            assert( 0 == state );
            state = STATE_INFILE;
         } else if( 0 == strncmp( argv[i], "-af", 3 ) ) {
            assert( 0 == state );
            state = STATE_ARCFILE;
         } else if( 0 == strncmp( argv[i], "-t", 2 ) ) {
            assert( 0 == state );
            state = STATE_TYPE;
         } else if( 0 == strncmp( argv[i], "-i", 2 ) ) {
            assert( 0 == state );
            state = STATE_ID;
         } else if( 0 == strncmp( argv[i], "-c", 2 ) ) {
            assert( 0 == state );
            assert( 0 == command );
            command = COMMAND_CREATE;
         } else if( 0 == strncmp( argv[i], "-a", 2 ) ) {
            assert( 0 == state );
            assert( 0 == command );
            command = COMMAND_ADD;
         } else if( 0 == strncmp( argv[i], "-l", 2 ) ) {
            assert( 0 == state );
            assert( 0 == command );
            command = COMMAND_LIST;
         }
         break;
      }
   }

   switch( command ) {
   case COMMAND_CREATE:
      drc_create( namebuf_arc );
      break;

   case COMMAND_ADD:
      assert( 0 != id );
      assert( 0 != *((uint32_t*)type_buf) );
      assert( NULL == file_contents );

      retval = dio_read_file( namebuf_in, &file_contents );
      if( 0 == retval ) {
         return 1;
      }

      file_sz = retval;
      retval = 0;

      assert( NULL != file_contents );
      assert( 0 < file_sz );

      /* TODO: Preprocess resource (convert tileset names into IDs. */
      /* TODO: Save a symbol->name map file to aid preprocessor later. */

      retval = drc_add_resource( namebuf_arc, *((uint32_t*)type_buf), 0,
         namebuf_in, strlen( namebuf_in ) + 1, file_contents, file_sz );
      if( 0 < retval ) {
         retval = 0;
      } else {
         /* Convert error code to positive. */
         retval *= -1;
      }
      break;

   case COMMAND_LIST:
      drc_list_resources( namebuf_arc, NULL );
      break;
   }

   return retval;
}

