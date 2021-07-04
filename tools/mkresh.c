
#include "../src/dio.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define STATE_INFILES   1
#define STATE_OUTHEADER 2
#define STATE_OUTRES    3
#define STATE_FMT       4
#define STATE_ID        5

#define FMT_PALM        1
#define FMT_WIN16       2

#define FILE_LIST_MAX   255

#define FOI_ID          0
#define FOI_FILENAME    1

const char gc_res_bitmap[] = "BITMAP";
const char gc_res_data_json[] = "DATA \"json\"";
const char gc_res_data_misc[] = "DATA \"misc\"";

int main( int argc, char* argv[] ) {
   int retval = 0;
   int i = 0,
      state = 0,
      fmt = 0,
      basename_start = 0,
      id_start = 0,
      filename_len = 0,
      extension_idx = 0;
   char* header_name = NULL,
      * file_list[FILE_LIST_MAX],
      * file_basename_list[FILE_LIST_MAX];
   const char* res_type = NULL;
   char namebuf_header[NAMEBUF_MAX + 1],
      namebuf_res[NAMEBUF_MAX + 1];
   size_t file_list_len = 0;
   FILE * header_file = NULL,
      * res_file = NULL;

   memset( namebuf_header, '\0', NAMEBUF_MAX + 1 );
   memset( namebuf_res, '\0', NAMEBUF_MAX + 1 );
   memset( file_list, '\0', sizeof( char* ) * FILE_LIST_MAX );

   for( i = 1 ; argc > i ; i++ ) {
      switch( state ) {
      case STATE_INFILES:
         if( '-' == argv[i][0] ) {
            /* Filenames don't start with -. */
            state = 0;
            break;
         }
         assert( NULL == file_list[file_list_len] );
         assert( file_list_len < FILE_LIST_MAX );
         filename_len = strlen( argv[i] );
         file_list[file_list_len] = calloc( filename_len + 1, 1 );
         assert( NULL != file_list[file_list_len] );
         strncpy( file_list[file_list_len], argv[i], filename_len );
         file_list_len++;
         break;

      case STATE_OUTHEADER:
         strncpy( namebuf_header, argv[i], NAMEBUF_MAX );
         state = 0;
         break;

      case STATE_OUTRES:
         strncpy( namebuf_res, argv[i], NAMEBUF_MAX );
         state = 0;
         break;

      case STATE_FMT:
         if( 0 == strncmp( argv[i], "palm", 4 ) ) {
            fmt = FMT_PALM;
         } else if( 0 == strncmp( argv[i], "win16", 5 ) ) {
            fmt = FMT_WIN16;
         }
         state = 0;
         break;

      case STATE_ID:
         id_start = atoi( argv[i] );
         state = 0;
         break;
      }

      if( 0 == state ) {
         if( 0 == strncmp( argv[i], "-if", 3 ) ) {
            state = STATE_INFILES;
         } else if( 0 == strncmp( argv[i], "-or", 3 ) ) {
            state = STATE_OUTRES;
         } else if( 0 == strncmp( argv[i], "-oh", 3 ) ) {
            state = STATE_OUTHEADER;
         } else if( 0 == strncmp( argv[i], "-f", 2 ) ) {
            state = STATE_FMT;
         } else if( 0 == strncmp( argv[i], "-i", 2 ) ) {
            state = STATE_ID;
         }
      }
   }

   assert( 0 != fmt );

   for( i = 0 ; file_list_len > i ; i++ ) {
      filename_len = strlen( file_list[i] );
      file_basename_list[i] = calloc( filename_len + 1, 1 );
      assert( NULL != file_basename_list[i] );

      /* Remove the path of the basename during copy. */
      basename_start = dio_basename( file_list[i], filename_len );
      strncpy( file_basename_list[i], &(file_list[i][basename_start]),
         filename_len - basename_start );
      filename_len = strlen( file_basename_list[i] );

      /* Remove the extension of the basename. */
      extension_idx = dio_char_idx_r(
         file_basename_list[i], filename_len, '.' );
      if( 0 < extension_idx ) {
         /* Turn the . into a NULL to chop off the extension. */
         file_basename_list[i][extension_idx] = '\0';
      }
   }

   if( 0 < strlen( namebuf_header ) ) {
      header_file = fopen( namebuf_header, "w" );
      assert( NULL!= header_file );

      fprintf( header_file, "\n#ifndef RESEXT_H\n#define RESEXT_H\n\n" );

      for( i = 0 ; file_list_len > i ; i++ ) {
         fprintf( header_file, "#define %s %d\n",
            file_basename_list[i], id_start + i );
      }

      fprintf( header_file, "\n#endif /* RESEXT_H */\n" );

      fclose( header_file );
      header_file = NULL;
   }

   if( 0 < strlen( namebuf_res ) ) {
      res_file = fopen( namebuf_res, "w" );
      assert( NULL!= res_file );

      for( i = 0 ; file_list_len > i ; i++ ) {
         /* TODO: Safety checks. */
         filename_len = strlen( file_list[i] );
         extension_idx = dio_char_idx_r( file_list[i], filename_len, '.' ) + 1;

         if( 0 == strncmp( &(file_list[i][extension_idx]), "bmp", 3 ) ) {
            res_type = gc_res_bitmap;
         } else if( 0 == strncmp( &(file_list[i][extension_idx]), "jso", 3 ) ) {
            res_type = gc_res_data_json;
         } else {
            res_type = gc_res_data_misc;
         }

         switch( fmt ) {
         case FMT_PALM:
            fprintf( res_file, "%s ID %s \"%s\"\n",
               res_type, file_basename_list[i], file_list[i] );
            break;

         case FMT_WIN16:
            fprintf( res_file, "%s %s \"%s\"\n",
               file_basename_list[i], res_type, file_list[i] );
            break;
         }

      }

      fclose( res_file );
      res_file = NULL;
   }

cleanup:

   if( NULL != header_file ) {
      fclose( header_file );
   }
   header_file = NULL;

   if( NULL != res_file ) {
      fclose( res_file );
   }
   header_file = NULL;

   return retval;
}

