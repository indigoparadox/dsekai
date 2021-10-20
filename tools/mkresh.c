
#include "../unilayer/unilayer.h"

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
#define FMT_FILE        3

#define RTYPE_MISC      0
#define RTYPE_JSON      1
#define RTYPE_BITMAP    2

#define FILE_LIST_MAX   255

#define FOI_ID          0
#define FOI_FILENAME    1

const char gc_win_res_bitmap[] = "BITMAP";
const char gc_palm_res_bitmap[] = "BITMAP";
const char gc_palm_res_data_json[] = "DATA \"json\"";
const char gc_palm_res_data_misc[] = "DATA \"misc\"";

int main( int argc, char* argv[] ) {
   int retval = 0;
   int i = 0,
      j = 0,
      state = 0,
      fmt = 0,
      rtype = RTYPE_MISC,
      basename_start = 0,
      id_start = 0,
      string_list_sz = 0,
      string_file_sz = 0,
      read = 0,
      filename_len = 0,
      extension_idx = 0;
   char* header_name = NULL,
      * file_list[FILE_LIST_MAX],
      * file_basename_list[FILE_LIST_MAX],
      * string_name_list[FILE_LIST_MAX],
      * string_list[FILE_LIST_MAX],
      last_c = '\0';
   const char* res_type = NULL;
   char namebuf_header[NAMEBUF_MAX + 1],
      namebuf_res[NAMEBUF_MAX + 1];
   size_t file_list_len = 0;
   FILE * header_file = NULL,
      * res_file = NULL,
      * string_file = NULL;

   memset( namebuf_header, '\0', NAMEBUF_MAX + 1 );
   memset( namebuf_res, '\0', NAMEBUF_MAX + 1 );
   memset( file_list, '\0', sizeof( char* ) * FILE_LIST_MAX );
   memset( string_list, '\0', sizeof( char* ) * FILE_LIST_MAX );
   memset( string_name_list, '\0', sizeof( char* ) * FILE_LIST_MAX );

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
         if( 0 == strncmp( argv[i], "file", 4 ) ) {
            fmt = FMT_FILE;
         } else if( 0 == strncmp( argv[i], "palm", 4 ) ) {
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

      /* Output header include guards. */
      fprintf( header_file, "\n#ifndef RESEXT_H\n#define RESEXT_H\n\n" );

      for( i = 0 ; file_list_len > i ; i++ ) {
         switch( fmt ) {
         case FMT_FILE:
            /* Hardcode the file path into the output header. */
            fprintf( header_file, "#define %s \"%s\"\n",
               file_basename_list[i], file_list[i] );
            break;

         default:
            /* Hardcode an arbitrary index/resource ID into the output header.
             */
            fprintf( header_file, "#define %s %d\n",
               file_basename_list[i], id_start + i );
            break;
         }
      }

      /* Output header include guard terminator. */
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
            rtype = RTYPE_BITMAP;
         } else if( 0 == strncmp( &(file_list[i][extension_idx]), "jso", 3 ) ) {
            rtype = RTYPE_JSON;
         }

         if( RTYPE_BITMAP == rtype ) {
            switch( fmt ) {
            case FMT_PALM:
               res_type = gc_palm_res_bitmap;
               break;
            case FMT_WIN16:
               res_type = gc_win_res_bitmap;
               break;
            }
         } else if( RTYPE_JSON == rtype ) {
            switch( fmt ) {
            case FMT_PALM:
               res_type = gc_palm_res_data_json;
               break;
            }
         } else {
            switch( fmt ) {
            case FMT_PALM:
               res_type = gc_palm_res_data_misc;
               break;
            }
         }

         if( FMT_WIN16 == fmt && RTYPE_JSON == rtype ) {

            assert( NULL == string_list[string_list_sz] );
            string_file = fopen( file_list[i], "r" );
            assert( NULL != string_file );
            fseek( string_file, 0, SEEK_END );
            string_file_sz = ftell( string_file ) + 1;
            fseek( string_file, 0, SEEK_SET );
            string_list[string_list_sz] = calloc( string_file_sz, 1 );
            string_name_list[string_list_sz] =
               calloc( strlen( file_basename_list[i] ) + 1, 1 );
            strcpy( string_name_list[string_list_sz], file_basename_list[i] );
            read = fread(
               string_list[string_list_sz], 1, string_file_sz, string_file );
            assert( read == string_file_sz - 1 );
            fclose( string_file );
            string_list_sz++;
            
         } else {
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
      }

      if( 0 < string_list_sz ) {
         fprintf( res_file, "STRINGTABLE\n{\n" );
         for( i = 0 ; string_list_sz > i ; i++ ) {
            fprintf( res_file, "   %s, \"", string_name_list[i] );
            for( j = 0 ; strlen( string_list[i] ) > j ; j++ ) {
               if( '"' == string_list[i][j] || '\\' == string_list[i][j] ) {
                  fputc( '\\', res_file );
               }
               if(
                  '\n' != string_list[i][j] && '\r' != string_list[i][j] &&
                  !(last_c == ' ' && string_list[i][j] == ' ')
               ) {
                  fputc( string_list[i][j], res_file );
               }
               last_c = string_list[i][j];
            }
            fprintf( res_file, "\"\n" );
         }
         fprintf( res_file, "}\n" );
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

