
#include "drc.h"

#include "dio.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define drc_copy( src, dest, sz, buffer ) \
   if( sizeof( buffer ) < sz ) { \
      retval = DRC_ERROR_BAD_BUFFER; \
      goto cleanup; \
   } \
   if( !fread( &buffer, sz, 1, src ) ) { \
      return DRC_ERROR_COULD_NOT_READ; \
   } \
   if( !fwrite( &buffer, 1, sz, dest ) ) { \
      return DRC_ERROR_COULD_NOT_WRITE; \
   }

#define drc_change( src, dest, type, new_val ) \
   fseek( src, sizeof( type ), SEEK_CUR ); \
   if( !fwrite( &new_val, sizeof( type ), 1, dest ) ) { \
      return DRC_ERROR_COULD_NOT_WRITE; \
   }

#define drc_skip( src, dest, type ) \
   fseek( src, sizeof( type ), SEEK_CUR ); \
   fseek( dest, sizeof( type ), SEEK_CUR );

static const uint32_t gc_zero_32 = 0;
static const uint16_t gc_drc_version = 1;

int32_t drc_list_resources( const char* path, struct DRC_TOC_E** ptoc ) {
   FILE* drc_file = NULL;
   int32_t toc_count_out = 0,
      i = 0;
   uint32_t toc_count = 0,
      toc_start = 0,
      toce_id = 0,
      toce_start = 0,
      toce_sz = 0;
   uint16_t toce_name_sz = 0;
   char toce_type[5],
      * toce_name = NULL;
   struct DRC_TOC_E toc_e_iter;

   dio_printf( "opening %s...\n", path );

   drc_file = fopen( path, "rb" );
   assert( NULL != drc_file );

   fseek( drc_file, DRC_HEADER_OFFSET_TOC_ENTRIES, SEEK_SET );
   fread( &toc_count, sizeof( uint32_t ), 1, drc_file );
   fseek( drc_file, DRC_HEADER_OFFSET_TOC_START, SEEK_SET );
   fread( &toc_start, sizeof( uint32_t ), 1, drc_file );
   fseek( drc_file, toc_start, SEEK_SET );
   assert( 0 != toc_start );
   for( i = 0 ; toc_count > i ; i++ ) {
      memset( toce_type, '\0', 5 );
      fread( &(toc_e_iter.type), sizeof( uint32_t ), 1, drc_file );
      fread( &(toc_e_iter.id), sizeof( uint32_t ), 1, drc_file );
      fread( &(toc_e_iter.data_start), sizeof( uint32_t ), 1, drc_file );
      fread( &(toc_e_iter.data_sz), sizeof( uint32_t ), 1, drc_file );
      fread( &(toc_e_iter.name_sz), sizeof( uint16_t ), 1, drc_file );
      toc_e_iter.name = calloc( toce_name_sz + 1, 1 );
      assert( NULL != toc_e_iter.name );
      fread( toc_e_iter.name, sizeof( char ), toc_e_iter.name_sz, drc_file );
      dio_printf( "%u: %s (%s, starts at %u bytes, %u bytes long)\n",
         toc_e_iter.id, toc_e_iter.name, (char*)&(toc_e_iter.type),
         toc_e_iter.data_start, toc_e_iter.data_sz );
      toc_count_out++;

      /* Store the entry in a list if requested. */
      if( NULL != ptoc ) {
         if( NULL == *ptoc ) {
            *ptoc = calloc( 1, sizeof( struct DRC_TOC_E ) );
            assert( NULL != *ptoc );
         } else {
            *ptoc =
               realloc( *ptoc, sizeof( struct DRC_TOC_E ) * toc_count_out );
            assert( NULL != *ptoc );
         }
         memcpy( &((*ptoc)[i]), &toc_e_iter, sizeof( struct DRC_TOC_E ) );
      }
   }

   fclose( drc_file );

   return toc_count_out;
}

int32_t drc_create( const char* path ) {
   FILE* drc_file = NULL;
   uint32_t file_sz = DRC_HEADER_SZ; /* Offset: Header End */

   drc_file = fopen( path, "wb" );
   if( NULL == drc_file ) {
      return DRC_ERROR_COULD_NOT_CREATE;
   }

   /* Set the TOC start and FES to just after the header.
    * They're both empty at this point.
    */
   fwrite( "DRCT", sizeof( char ), 4, drc_file );              /* Type */
   fwrite( &gc_drc_version, sizeof( uint16_t ), 1, drc_file ); /* Version */
   fwrite( &gc_zero_32, sizeof( uint32_t ), 1, drc_file );     /* CRC32 */
   fwrite( &file_sz, sizeof( uint32_t ), 1, drc_file );        /* Filesize */
   fwrite( &file_sz, sizeof( uint32_t ), 1, drc_file );     /* TOC Strt */
   fwrite( &gc_zero_32, sizeof( uint32_t ), 1, drc_file );     /* TOC Ct */
   fwrite( &gc_zero_32, sizeof( uint32_t ), 1, drc_file );     /* Reserved */
   fwrite( &file_sz, sizeof( uint32_t ), 1, drc_file );     /* FES */

   assert( ftell( drc_file ) == DRC_HEADER_SZ );
   assert( ftell( drc_file ) == file_sz );

   fclose( drc_file );

   return file_sz;
}

/**
 * @return Offset of the newly added resource on success. <0 otherwise.
 */
int32_t drc_add_resource(
   const char* path, uint32_t type, uint32_t id,
   const char* name, uint16_t name_sz, const uint8_t* buffer, uint32_t buffer_sz
) {
   FILE* drc_file = NULL,
      * drc_file_tmp = NULL;
   int32_t retval = 0;
   char* temp_dir = NULL,
      * toc_iter_name = NULL;
   char tmp_path[DRC_MAX_PATH + 1];
   int32_t existing_offset = 0,
      toc_offset = 0,
      i = 0;
   uint32_t data_offset = 0,
      toc_iter_type = 0,
      toc_iter_id = 0,
      toc_iter_start = 0,
      toc_iter_size = 0,
      toc_new_offset = 0,
      toc_entries = 0,
      new_sz = 0,
      read = 0,
      read_total_data = 0;
   uint16_t toc_iter_name_sz = 0;
   uint8_t copy_buffer[DRC_COPY_BLOCK_SZ + 1];

   if( strlen( path ) > DRC_MAX_PATH ) {
      return DRC_ERROR_PATH_TOO_LONG;
   }

   /* Make sure resource does not exist, get offset if it does. */
   existing_offset = drc_get_resource( path, type, id, NULL );
   if( 0 > existing_offset ) {
      /* Error occurred. */
      return existing_offset;
   } else if( 0 < existing_offset ) {
      /* Resource exists. */
      dio_printf( "resource %u/%u exists at %d; replacing...\n",
         type, id, existing_offset );
      existing_offset = drc_remove_resource( path, type, id );
      if( 0 < existing_offset ) {
         /* Error occurred. */
         return existing_offset;
      }
   }

   /* Create a temp copy to modify. */
   temp_dir = getenv( "TEMP" );
   if( NULL == temp_dir ) {
      temp_dir = DRC_DEFAULT_TEMP;
   }
   memset( tmp_path, '\0', DRC_MAX_PATH + 1 );
   strncpy( tmp_path, temp_dir, DRC_MAX_PATH );
   tmp_path[strlen( temp_dir )] = DRC_PATH_SEP;
   strncpy( &(tmp_path[strlen( temp_dir ) + 1]), "drctemp.tmp",
      DRC_MAX_PATH - strlen( temp_dir + 1 ) );
   dio_printf( "creating temporary copy: %s\n", tmp_path );
   drc_file_tmp = fopen( tmp_path, "wb" );
   if( NULL == drc_file_tmp ) {
      return DRC_ERROR_COULD_NOT_CREATE_TEMP;
   }

   /* Read the existing file, inserting new information as we go. */
   drc_file = fopen( path, "rb" );
   if( NULL == drc_file ) {
      return DRC_ERROR_COULD_NOT_OPEN;
   }

   /* Grab the current TOC count. */
   fseek( drc_file, DRC_HEADER_OFFSET_TOC_ENTRIES, SEEK_SET );
   fread( &toc_entries, sizeof( uint32_t ), 1, drc_file );

   /* Get the size of the existing file. */
   data_offset = drc_get_end( path );
   fseek( drc_file, DRC_HEADER_OFFSET_FILESIZE, SEEK_SET );

   dio_printf( "existing archive is %d bytes w/ %u entries\n",
      data_offset, toc_entries );

   /* We're adding one. */
   toc_entries++;

   /* Reset source file to match dest starting point. */
   fseek( drc_file, 0, SEEK_SET );

   /* Copy the header. */
   drc_copy( drc_file, drc_file_tmp, sizeof( uint32_t ),          /* Type */
      copy_buffer );
   drc_copy( drc_file, drc_file_tmp, sizeof( uint16_t ),          /* Version */
      copy_buffer );
   if( 1 != *((uint16_t*)&copy_buffer) ) {
      retval = DRC_ERROR_BAD_VERSION;
      goto cleanup;
   }
   drc_change( drc_file, drc_file_tmp, uint32_t, gc_zero_32 );    /* CRC32 */
   drc_change( drc_file, drc_file_tmp, uint32_t, gc_zero_32 );    /* Filesize */
   drc_copy( drc_file, drc_file_tmp, sizeof( uint32_t ),          /* TOC Strt */
      copy_buffer );
   drc_change( drc_file, drc_file_tmp, uint32_t, toc_entries );   /* TOC Ct */
   drc_copy( drc_file, drc_file_tmp, sizeof( uint32_t ),          /* Reserved */
      copy_buffer );
   drc_change( drc_file, drc_file_tmp, uint32_t, gc_zero_32 );    /* Dat Strt */

   if( DRC_HEADER_SZ != ftell( drc_file ) ) {
      retval = DRC_ERROR_COULD_NOT_READ;
      goto cleanup;
   }
   if( DRC_HEADER_SZ != ftell( drc_file_tmp ) ) {
      retval = DRC_ERROR_COULD_NOT_WRITE;
      goto cleanup;
   }

   /* Copy the TOC. */
   for( i = 0 ; toc_entries - 1 > i ; i++ ) {
      drc_copy( drc_file, drc_file_tmp, sizeof( uint32_t ), toc_iter_type );
      drc_copy( drc_file, drc_file_tmp, sizeof( uint32_t ), toc_iter_id );
      drc_copy( drc_file, drc_file_tmp, sizeof( uint32_t ), toc_iter_start );
      drc_copy( drc_file, drc_file_tmp, sizeof( uint32_t ), toc_iter_size );
      drc_copy( drc_file, drc_file_tmp, sizeof( uint16_t ), toc_iter_name_sz );

      /* Copy the name. */
      toc_iter_name = calloc( 1, toc_iter_name_sz );
      if( NULL == toc_iter_name ) {
         retval = DRC_ERROR_COULD_NOT_ALLOC;
         goto cleanup;
      }
      if( toc_iter_name_sz < fread(
         toc_iter_name, 1, toc_iter_name_sz, drc_file )
      ) {
         retval = DRC_ERROR_COULD_NOT_READ;
         goto cleanup;
      }
      if( toc_iter_name_sz < fwrite(
         toc_iter_name, 1, toc_iter_name_sz, drc_file_tmp )
      ) {
         retval = DRC_ERROR_COULD_NOT_WRITE;
         goto cleanup;
      }
      
      dio_printf( "skipping TOC entry %d, %s (starting at %u, %u bytes)..\n",
         toc_iter_id, toc_iter_name, toc_iter_start, toc_iter_size );

      free( toc_iter_name );
      toc_iter_name = NULL;
   }

   /* Write the new TOC entry. */
   dio_printf( "creating new TOC entry (%u) for file %u, %s (%u bytes)...\n",
      toc_entries - 1, id, name, buffer_sz );
   toc_new_offset = ftell( drc_file_tmp );
   fwrite( &type, sizeof( uint32_t ), 1, drc_file_tmp );
   fwrite( &id, sizeof( uint32_t ), 1, drc_file_tmp );
   fwrite( &gc_zero_32, sizeof( uint32_t ), 1, drc_file_tmp ); /* Come back. */
   fwrite( &buffer_sz, sizeof( uint32_t ), 1, drc_file_tmp );
   fwrite( &name_sz, sizeof( uint16_t ), 1, drc_file_tmp );
   fwrite( name, 1, name_sz, drc_file_tmp );

   /* Update the data offset. */
   data_offset = ftell( drc_file_tmp );
   dio_printf( "new data offset is %u...\n", data_offset );
   fseek( drc_file_tmp, DRC_HEADER_OFFSET_FES, SEEK_SET );
   fwrite( &data_offset, sizeof( uint32_t ), 1, drc_file_tmp );
   fseek( drc_file_tmp, data_offset, SEEK_SET );

   /* Start copying data. */
   if( data_offset != ftell( drc_file_tmp ) ) {
      retval = DRC_ERROR_COULD_NOT_WRITE;
      goto cleanup;
   }
   dio_printf( "copying existing data...\n" );
   while(
      0 != (read = fread( copy_buffer, 1, DRC_COPY_BLOCK_SZ, drc_file ))
   ) {
      read_total_data += read;
      fwrite( copy_buffer, 1, read, drc_file_tmp );
   }
   dio_printf( "copied %u bytes\n", read_total_data );

   /* Update the TOC entry offset. */
   data_offset = ftell( drc_file_tmp );
   dio_printf( "new entry's data offset is %u...\n", data_offset );
   fseek( drc_file_tmp, toc_new_offset + 8, /* 8 bytes in. */
      SEEK_SET );
   fwrite( &data_offset, sizeof( uint32_t ), 1, drc_file_tmp );

   /* Append the new data. */
   fseek( drc_file_tmp, data_offset, SEEK_SET );
   assert( data_offset == ftell( drc_file_tmp ) );
   fwrite( buffer, sizeof( uint8_t ), buffer_sz, drc_file_tmp );
   retval = data_offset;

   /* Update the filesize. */
   new_sz = ftell( drc_file_tmp );
   dio_printf( "new archive size is %u bytes\n", new_sz );
   fseek( drc_file_tmp, DRC_HEADER_OFFSET_FILESIZE, SEEK_SET );
   fwrite( &new_sz, sizeof( uint32_t ), 1, drc_file_tmp );

cleanup:

   if( NULL != drc_file ) {
      fclose( drc_file );
   }

   if( NULL != drc_file_tmp ) {
      fclose( drc_file_tmp );
   }

   if( NULL != toc_iter_name ) {
      free( toc_iter_name );
   }

   if( 0 < retval && 0 > dio_move_file( tmp_path, path ) ) {
      retval = DRC_ERROR_COULD_NOT_MOVE_TEMP;
   }
   
   return retval;
}

int32_t drc_remove_resource(
   const char* path, uint32_t type, uint32_t id
) {
   uint32_t removed_offset = 0;

   return removed_offset;
}

/**
 * @return Size (in bytes) of resource if found, or 0 or less otherwise.
 */
int32_t drc_get_resource(
   const char* path, uint32_t type, uint32_t id, uint8_t** buffer
) {
   FILE* drc_file = NULL;
   int32_t drc_file_sz = 0,
      i = 0,
      resource_sz = 0;
   uint32_t toc_count = 0,
      toc_start = 0,
      toce_id = 0,
      toce_start = 0,
      toce_sz = 0,
      toce_type = 0;
   uint16_t toce_name_sz = 0;

   dio_printf( "opening %s...\n", path );

   drc_file = fopen( path, "rb" );
   assert( NULL != drc_file );

   fseek( drc_file, DRC_HEADER_OFFSET_TOC_ENTRIES, SEEK_SET );
   fread( &toc_count, sizeof( uint32_t ), 1, drc_file );
   fseek( drc_file, DRC_HEADER_OFFSET_TOC_START, SEEK_SET );
   fread( &toc_start, sizeof( uint32_t ), 1, drc_file );
   fseek( drc_file, toc_start, SEEK_SET );
   assert( 0 != toc_start );
   for( i = 0 ; toc_count > i ; i++ ) {
      memset( &toce_type, '\0', 5 );
      fread( &toce_type, sizeof( uint32_t ), 1, drc_file );
      fread( &toce_id, sizeof( uint32_t ), 1, drc_file );
      fread( &toce_start, sizeof( uint32_t ), 1, drc_file );
      fread( &toce_sz, sizeof( uint32_t ), 1, drc_file );
      fread( &toce_name_sz, sizeof( uint16_t ), 1, drc_file );
      fseek( drc_file, toce_name_sz, SEEK_CUR );

      if( toce_type == type && toce_id == id ) {
         /* Resource found. */
         dio_printf( "found resource %u (at %u bytes, %u bytes long...\n",
            toce_id, toce_start, toce_sz );
         resource_sz = toce_sz;
         if( NULL != buffer ) {
            /* Load file contents into buffer at referencd address. */
            *buffer = calloc( 1, toce_sz );
            if( NULL == *buffer ) {
               resource_sz = -1;
            } else {
               fseek( drc_file, toce_start, SEEK_SET );
               fread( *buffer, toce_sz, 1, drc_file );
            }
         }
         break;
      }
   }

   fclose( drc_file );

   return resource_sz;
}

int32_t drc_get_end( const char* path ) {
   FILE* drc_file = NULL;
   uint32_t drc_file_sz = 0;

   drc_file = fopen( path, "rb" );
   if( NULL == drc_file ) {
      return DRC_ERROR_COULD_NOT_OPEN;
   }

   fseek( drc_file, DRC_HEADER_OFFSET_FILESIZE, SEEK_SET );

   fread( &drc_file_sz, 4, 1, drc_file );

   fclose( drc_file );

   return drc_file_sz;
}

