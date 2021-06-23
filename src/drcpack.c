
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
#define STATE_HEADER    5

#define COMMAND_CREATE  1
#define COMMAND_ADD     2
#define COMMAND_LIST    3
#define COMMAND_HEADER  4

int main( int argc, char* argv[] ) {
   int i = 0,
      retval = 0,
      state = 0,
      command = 0,
      extension_idx = 0;
   uint32_t id = 0,
      file_sz = 0,
      basename_sz = 0;
   char namebuf_in[NAMEBUF_MAX + 1],
      namebuf_arc[NAMEBUF_MAX + 1],
      type_buf[5];
   uint8_t* file_contents = NULL;
   struct DRC_TOC_E* toc_entries = NULL;
   int32_t toc_entries_sz = 0;
   FILE* header_file = NULL;

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

      case STATE_HEADER:
         strncpy( namebuf_in, argv[i], NAMEBUF_MAX );
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
         } else if( 0 == strncmp( argv[i], "-lh", 3 ) ) {
            assert( 0 == state );
            assert( 0 == command );
            state = STATE_HEADER;
            command = COMMAND_HEADER;
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

      basename_sz = dio_basename( namebuf_in, strlen( namebuf_in ) );

      retval = drc_add_resource( namebuf_arc, *((uint32_t*)type_buf), 0,
         &(namebuf_in[basename_sz]),
         strlen( namebuf_in ) - basename_sz, file_contents, file_sz );
      if( 0 <= retval ) {
         retval = 0;
      } else {
         /* Convert error code to positive. */
         dio_eprintf( "error adding resource: %d\n", retval );
         retval *= -1;
      }
      break;

   case COMMAND_LIST:
      drc_list_resources( namebuf_arc, NULL );
      break;
   
   case COMMAND_HEADER:
      toc_entries_sz = drc_list_resources( namebuf_arc, &toc_entries );
      header_file = fopen( namebuf_in, "w" );
      for( i = 0 ; toc_entries_sz > i ; i++ ) {
         extension_idx = dio_char_idx_r(
            toc_entries[i].name, toc_entries[i].name_sz, '.' );
         if( 0 < extension_idx ) {
            /* Turn the . into a NULL to chop off the extension. */
            toc_entries[i].name[extension_idx] = '\0';
         }

         fprintf( header_file, "#define %s 0x%08x\n",
            toc_entries[i].name, toc_entries[i].id );
         free( toc_entries[i].name );
      }
      free( toc_entries );
      fclose( header_file );
      break;
   }

   return retval;
}

