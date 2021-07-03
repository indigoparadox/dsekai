
#include "data/drcwrite.h"
#include "../src/data/dio.h"
#include "../src/memory.h"

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
   FILE* extract_res_file = NULL;
   int i = 0,
      retval = 0,
      state = 0,
      command = 0,
      command_create = 0,
      command_add = 0,
      command_list = 0,
      command_header = 0,
      command_extract = 0,
      filename_len = 0,
      extension_idx = 0;
   uint32_t id = 0,
      extract_res_name_sz = 0,
      wrote = 0,
      file_sz = 0,
      basename_sz = 0;
   int32_t extract_res_buffer_sz = 0;
   char namebuf_header[NAMEBUF_MAX + 1],
      namebuf_arc[NAMEBUF_MAX + 1],
      tmp_path[DIO_PATH_MAX + 1];
   char* extract_res_name = NULL,
      * temp_dir = NULL;
   uint8_t* file_contents = NULL,
      * extract_res_buffer = NULL;
   struct DRC_TOC_E* toc_entries = NULL;
   int32_t toc_entries_sz = 0;
   FILE* header_file = NULL;
   size_t file_list_len = 0;
   uint8_t* res_buffer = NULL;
   union DRC_TYPE typebuf;
   union DRC_TYPE typebuf_extract;
   struct DIO_STREAM drc_file_in,
      drc_file_out;

   memset( namebuf_header, '\0', NAMEBUF_MAX + 1 );
   memset( namebuf_arc, '\0', NAMEBUF_MAX + 1 );
   memset( file_list, '\0', sizeof( char* ) * FILE_LIST_MAX );
   memset( &typebuf, '\0', 4 );
   memset( &typebuf_extract, '\0', 4 );

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
         filename_len = strlen( argv[i] );
         file_list[file_list_len] = memory_alloc( filename_len + 1, 1 );
         assert( NULL != file_list[file_list_len] );
         strncpy( file_list[file_list_len], argv[i], filename_len );
         file_list_len++;
         break;

      case STATE_ARCFILE:
         strncpy( namebuf_arc, argv[i], NAMEBUF_MAX );
         state = 0;
         break;

      case STATE_TYPE:
         strncpy( typebuf_extract.str, argv[i], 4 );
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

         } else if( 0 == strncmp( argv[i], "-x", 2 ) ) {
            state = 0;
            command_extract = 1;
         }
      }
   }

   if( command_create ) {
      debug_printf( 2, "command: create" );
      dio_open_stream_file( namebuf_arc, "wb", &drc_file_out );
      if(
         0 == dio_type_stream( &drc_file_out ) ||
         0 > drc_create( &drc_file_out )
      ) {
         retval = DRC_ERROR_COULD_NOT_CREATE;
         goto cleanup;
      }
      dio_close_stream( &drc_file_out );
   }

   if( command_add ) {
      debug_printf( 2, "command: add" );

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

         extension_idx = dio_char_idx_r(
            file_list[i], strlen( file_list[i] ), '.' );
         if(
            0 == strncmp( "bmp", &(file_list[i][extension_idx + 1]), 3 ) ||
            0 == strncmp( "cga", &(file_list[i][extension_idx + 1]), 3 )
         ) {
            typebuf.str[0] = 'B';
            typebuf.str[1] = 'M';
            typebuf.str[2] = 'P';
            typebuf.str[3] = '1';
         } else if(
            0 == strncmp( "jso", &(file_list[i][extension_idx + 1]), 3 )
         ) {
            typebuf.str[0] = 'j';
            typebuf.str[1] = 's';
            typebuf.str[2] = 'o';
            typebuf.str[3] = 'n';
         }

         /* Open archive to copy. */
         dio_open_stream_file( namebuf_arc, "rb", &drc_file_in );
         if( 0 == dio_type_stream( &drc_file_in ) ) {
            error_printf( "could not open archive" );
            retval = DRC_ERROR_COULD_NOT_OPEN;
            goto cleanup;
         }
         
         /* Create a temp copy to modify. */
         if( 0 >= dio_mktemp_path( tmp_path, DIO_PATH_MAX, "drctmp.drc" ) ) {
            error_printf( "could not create temporary path" );
            retval = -1;
            goto cleanup;
         }

         debug_printf( 2, "creating temporary copy: %s", tmp_path );
         dio_open_stream_file( tmp_path, "wb", &drc_file_out );
         if( 0 == dio_type_stream( &drc_file_out ) ) {
            error_printf( "could not create temporary file" );
            retval = DRC_ERROR_COULD_NOT_CREATE_TEMP;
            goto cleanup;
         }

         retval = drc_add_resource( &drc_file_in, &drc_file_out, typebuf, id,
            &(file_list[i][basename_sz]),
            strlen( file_list[i] ) - basename_sz, file_contents, file_sz );
         if( 0 <= retval ) {
            retval = 0;
         } else {
            /* Convert error code to positive. */
            error_printf( "error adding resource: %d", retval );
            retval *= -1;
            goto cleanup;
         }

         /* Close the archive and replace the original with the temp out. */
         dio_close_stream( &drc_file_in );
         dio_close_stream( &drc_file_out );
         debug_printf( 2, "moving temporary copy %s to %s...",
            tmp_path, namebuf_arc );
         if( 0 > dio_move_file( tmp_path, namebuf_arc ) ) {
            retval = DRC_ERROR_COULD_NOT_MOVE_TEMP;
            goto cleanup;
         }

         id++;
      }
   }

   if( command_list ) {
      debug_printf( 2, "command: list" );
      dio_open_stream_file( namebuf_arc, "rb", &drc_file_in );
      if( 0 == dio_type_stream( &drc_file_in ) ) {
         retval = DRC_ERROR_COULD_NOT_OPEN;
         goto cleanup;
      }
      toc_entries_sz = drc_list_resources( &drc_file_in, &toc_entries, 0 );
      dio_close_stream( &drc_file_in );

      for( i = 0 ; toc_entries_sz > i ; i++ ) {
         printf( "TOC entry %d | type %c%c%c%c | size %d bytes @ offset %d\n",
            toc_entries[i].id,
            toc_entries[i].type.str[0], toc_entries[i].type.str[1],
            toc_entries[i].type.str[2], toc_entries[i].type.str[3],
            toc_entries[i].data_sz, toc_entries[i].data_start );
      }

      free( toc_entries );
      toc_entries = NULL;
      toc_entries_sz = 0;
   }

   if( command_extract ) {
      debug_printf( 2, "command: extract" );
      
      dio_open_stream_file( namebuf_arc, "rb", &drc_file_in );
      if( 0 == dio_type_stream( &drc_file_in ) ) {
         retval = DRC_ERROR_COULD_NOT_OPEN;
         goto cleanup;
      }
      extract_res_name_sz = drc_get_resource_name(
         &drc_file_in, typebuf_extract, id, &extract_res_name );
      assert( 0 < extract_res_name_sz );
      assert( NULL != extract_res_name );

      extract_res_buffer_sz = drc_get_resource_sz(
         &drc_file_in, typebuf_extract, id );
      if( 0 >= extract_res_buffer_sz ) {
         error_printf( "unable to find requested resource" );
      }
      extract_res_buffer = memory_alloc( 1, extract_res_buffer_sz );

      retval = drc_get_resource(
         &drc_file_in, typebuf_extract, id,
         extract_res_buffer, extract_res_name_sz );
      if( 0 > retval ) {
         error_printf( "unable to extract file" );
         goto cleanup;
      }
      dio_close_stream( &drc_file_in );

      extract_res_file = fopen( extract_res_name, "wb" );
      assert( NULL != extract_res_file );
      debug_printf( 2, "writing resource file..." );
      wrote = fwrite( extract_res_buffer, 1, retval, extract_res_file );
      debug_printf( 2, 
         "wrote %u (%ld) bytes", wrote, ftell( extract_res_file ) );
      assert( wrote == retval );

      retval = 0;
   }
   
   if( command_header ) {
      
      debug_printf( 2, "command: header" );

      dio_open_stream_file( namebuf_arc, "rb", &drc_file_in );
      if( 0 == dio_type_stream( &drc_file_in ) ) {
         error_printf( "error opening header file" );
         retval = DRC_ERROR_COULD_NOT_OPEN;
         goto cleanup;
      }
      toc_entries_sz = drc_list_resources( &drc_file_in, &toc_entries, 0 );
      dio_close_stream( &drc_file_in );
      
      debug_printf( 2, "opening header file..." );
      header_file = fopen( namebuf_header, "w" );
      
      assert( NULL != header_file );
      /* TODO: Dynamically generate include guard name. */
      fprintf( header_file, "#ifndef RESEXT_H\n" );
      fprintf( header_file, "#define RESEXT_H\n\n" );
      for( i = 0 ; toc_entries_sz > i ; i++ ) {
         debug_printf( 1, "writing TOC entry %d (ID %d) to header...",
            i, toc_entries[i].id );
         extension_idx = dio_char_idx_r(
            toc_entries[i].name, toc_entries[i].name_sz, '.' );
         if( 0 < extension_idx ) {
            /* Turn the . into a NULL to chop off the extension. */
            toc_entries[i].name[extension_idx] = '\0';
         }

         fprintf( header_file, "#define %s %u\n",
            toc_entries[i].name, toc_entries[i].id );
      }
      free( toc_entries );
      fprintf( header_file, "\n#endif /* RESEXT_H */\n" );
      fclose( header_file );
   }

   assert( 0 == retval );

cleanup:

   debug_printf( 2, "cleaning up resources..." );

   if( 0 != dio_type_stream( &drc_file_in ) ) {
      dio_close_stream( &drc_file_in );
   }

   if( 0 != dio_type_stream( &drc_file_out ) ) {
      dio_close_stream( &drc_file_out );
   }

   if( NULL != extract_res_file ) {
      fclose( extract_res_file );
   }

   if( NULL != extract_res_name ) {
      free( extract_res_name );
   }

   if( NULL != extract_res_buffer ) {
      free( extract_res_buffer );
   }

   for( i = 0 ; file_list_len > i ; i++ ) {
      free( file_list[i] );
   }

   return retval;
}

