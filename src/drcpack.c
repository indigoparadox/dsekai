
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

#define FILE_LIST_MAX   255

int main( int argc, char* argv[] ) {
   char* file_list[FILE_LIST_MAX];
   int i = 0,
      retval = 0,
      state = 0,
      command = 0,
      command_create = 0,
      command_add = 0,
      command_list = 0,
      command_header = 0,
      extension_idx = 0;
   uint32_t id = 0,
      file_sz = 0,
      basename_sz = 0;
   char namebuf_header[NAMEBUF_MAX + 1],
      namebuf_arc[NAMEBUF_MAX + 1],
      type_buf[5];
   uint8_t* file_contents = NULL;
   struct DRC_TOC_E* toc_entries = NULL;
   int32_t toc_entries_sz = 0;
   FILE* header_file = NULL;
   size_t file_list_len = 0;

   memset( namebuf_header, '\0', NAMEBUF_MAX + 1 );
   memset( namebuf_arc, '\0', NAMEBUF_MAX + 1 );
   memset( type_buf, '\0', 5 );
   memset( file_list, '\0', sizeof( char* ) * FILE_LIST_MAX );

   for( i = 1 ; argc > i ; i++ ) {
      switch( state ) {
      case STATE_INFILE:
         if( '-' == argv[i][0] ) {
            /* Filenames don't start with -. */
            state = 0;
            break;
         }
         assert( NULL == file_list[file_list_len] );
         assert( file_list_len < FILE_LIST_MAX );
         file_list[file_list_len] = calloc( NAMEBUF_MAX + 1, 1 );
         assert( NULL != file_list[file_list_len] );
         strncpy( file_list[file_list_len], argv[i], NAMEBUF_MAX );
         file_list_len++;
         printf( "%d: (l:%lu) %s\n", i, file_list_len, file_list[file_list_len - 1] );
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
         id = atoi( argv[i] );
         state = 0;
         break;

      case STATE_HEADER:
         strncpy( namebuf_header, argv[i], NAMEBUF_MAX );
         state = 0;
         break;
      }

      if( 0 == state ) {
         if( 0 == strncmp( argv[i], "-if", 3 ) ) {
            state = STATE_INFILE;
         } else if( 0 == strncmp( argv[i], "-af", 3 ) ) {
            state = STATE_ARCFILE;
         } else if( 0 == strncmp( argv[i], "-t", 2 ) ) {
            state = STATE_TYPE;
         } else if( 0 == strncmp( argv[i], "-i", 2 ) ) {
            state = STATE_ID;

         } else if( 0 == strncmp( argv[i], "-c", 2 ) ) {
            state = 0;
            command_create = 1;

         } else if( 0 == strncmp( argv[i], "-a", 2 ) ) {
            state = 0;
            command_add = 1;

         } else if( 0 == strncmp( argv[i], "-lh", 3 ) ) {
            state = STATE_HEADER;
            command_header = 1;

         } else if( 0 == strncmp( argv[i], "-l", 2 ) ) {
            state = 0;
            command_list = 1;
         }
      }
   }

   if( command_create ) {
      if( 0 > drc_create( namebuf_arc ) ) {
         return 1;
      }
   }

   if( command_add ) {
      assert( 0 != *((uint32_t*)type_buf) );
      assert( NULL == file_contents );

      for( i = 0 ; file_list_len > i ; i++ ) {
         retval = dio_read_file( file_list[i], &file_contents );
         if( 0 == retval ) {
            return 1;
         }

         file_sz = retval;
         retval = 0;

         assert( NULL != file_contents );
         assert( 0 < file_sz );

         /* TODO: Preprocess resource (convert tileset names into IDs. */
         /* TODO: Save a symbol->name map file to aid preprocessor later. */

         basename_sz = dio_basename( file_list[i], strlen( file_list[i] ) );

         retval = drc_add_resource( namebuf_arc, *((uint32_t*)type_buf), id,
            &(file_list[i][basename_sz]),
            strlen( file_list[i] ) - basename_sz, file_contents, file_sz );
         if( 0 <= retval ) {
            retval = 0;
         } else {
            /* Convert error code to positive. */
            dio_eprintf( "error adding resource: %d\n", retval );
            retval *= -1;
         }

         id++;
      }
   }

   if( command_list ) {
      drc_list_resources( namebuf_arc, NULL );
   }
   
   if( command_header ) {
      toc_entries_sz = drc_list_resources( namebuf_arc, &toc_entries );
      header_file = fopen( namebuf_header, "w" );
      assert( NULL != header_file );
      /* TODO: Dynamically generate include guard name. */
      fprintf( header_file, "#ifndef RESEXT_H\n" );
      fprintf( header_file, "#define RESEXT_H\n\n" );
      for( i = 0 ; toc_entries_sz > i ; i++ ) {
         extension_idx = dio_char_idx_r(
            toc_entries[i].name, toc_entries[i].name_sz, '.' );
         if( 0 < extension_idx ) {
            /* Turn the . into a NULL to chop off the extension. */
            toc_entries[i].name[extension_idx] = '\0';
         }

         fprintf( header_file, "#define %s %u\n",
            toc_entries[i].name, toc_entries[i].id );
         free( toc_entries[i].name );
      }
      free( toc_entries );
      fprintf( header_file, "\n#endif /* RESEXT_H */\n" );
      fclose( header_file );
   }

   for( i = 0 ; file_list_len > i ; i++ ) {
      free( file_list[i] );
   }

   return retval;
}

