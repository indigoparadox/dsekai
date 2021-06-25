
#include "drc.h"

#include "../memory.h"
#include "dio.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define drc_copy( src, dest, sz, buffer ) \
   assert( sizeof( buffer ) >= sz ); \
   if( sizeof( buffer ) < sz ) { \
      retval = DRC_ERROR_BAD_BUFFER; \
      goto cleanup; \
   } \
   if( sz != fread( &buffer, 1, sz, src ) ) { \
      assert( 1 == 0 ); \
      return DRC_ERROR_COULD_NOT_READ; \
   } \
   if( sz != fwrite( &buffer, 1, sz, dest ) ) { \
      assert( 1 == 0 ); \
      return DRC_ERROR_COULD_NOT_WRITE; \
   }

#define drc_change( src, dest, type, new_val ) \
   fseek( src, sizeof( type ), SEEK_CUR ); \
   if( sizeof( type ) != fwrite( &new_val, 1, sizeof( type ), dest ) ) { \
      assert( 1 == 0 ); \
      return DRC_ERROR_COULD_NOT_WRITE; \
   }

#define drc_skip( src, dest, type ) \
   fseek( src, sizeof( type ), SEEK_CUR ); \
   fseek( dest, sizeof( type ), SEEK_CUR );

static const uint8_t gc_zero_8 = 0;
static const uint32_t gc_zero_32 = 0;
static const uint16_t gc_drc_version = 1;

static int32_t drc_read_toc_e( FILE* drc_file, struct DRC_TOC_E* toc_e ) {
   uint32_t toc_e_start = 0;

   assert( NULL == toc_e->name );
   if( NULL != toc_e->name ) {
      memory_free( &toc_e->name );
      toc_e->name = NULL;
   }

   toc_e_start = ftell( drc_file );
   memset( toc_e, '\0', sizeof( struct DRC_TOC_E ) );

   fread( &(toc_e->type), sizeof( uint32_t ), 1, drc_file );
   fread( &(toc_e->reserved), sizeof( uint32_t ), 1, drc_file );
   
   assert( ftell( drc_file ) == toc_e_start + DRC_TOC_E_OFFSET_ID );
   fread( &(toc_e->id), sizeof( uint32_t ), 1, drc_file );
   assert( 10000 > toc_e->id );

   assert( ftell( drc_file ) == toc_e_start + DRC_TOC_E_OFFSET_DATA_START );
   fread( &(toc_e->data_start), sizeof( uint32_t ), 1, drc_file );
   
   assert( ftell( drc_file ) == toc_e_start + DRC_TOC_E_OFFSET_DATA_SZ );
   fread( &(toc_e->data_sz), sizeof( uint32_t ), 1, drc_file );
   
   assert( ftell( drc_file ) == toc_e_start + DRC_TOC_E_OFFSET_NAME_SZ );
   fread( &(toc_e->name_sz), sizeof( uint16_t ), 1, drc_file );
   assert( 0 < toc_e->name_sz );
   toc_e->name = memory_alloc( toc_e->name_sz + 1, 1 );
   assert( NULL != toc_e->name );

   dio_printf( "read TOC entry (%ld bytes)\n",
      ftell( drc_file ) - toc_e_start );
   assert( DRC_TOC_E_SZ == ftell( drc_file ) - toc_e_start );

   fread( toc_e->name, sizeof( char ), toc_e->name_sz, drc_file );
   
   dio_printf( "%u: %s (%s, starts at %u bytes, %u bytes long)\n",
      toc_e->id, toc_e->name, (char*)&(toc_e->type),
      toc_e->data_start, toc_e->data_sz );

   assert( ftell( drc_file ) == toc_e_start + DRC_TOC_E_SZ + toc_e->name_sz );

   /* TODO: Error handling .*/
   return 0;
}

static int32_t drc_read_header( FILE* drc_file, struct DRC_HEADER* h ) {
   fread( &h->type, sizeof( uint32_t ), 1, drc_file );
   fread( &h->version, sizeof( uint16_t ), 1, drc_file );
   assert( 1 == h->version );
   fseek( drc_file, DRC_HEADER_OFFSET_FILESIZE, SEEK_SET );
   fread( &h->filesize, sizeof( uint32_t ), 1, drc_file );
   fread( &h->toc_start, sizeof( uint32_t ), 1, drc_file );
   fread( &h->toc_entries, sizeof( uint32_t ), 1, drc_file );

   fseek( drc_file, 0, SEEK_END );
   assert( ftell( drc_file ) == h->filesize );

   fseek( drc_file, h->toc_start, SEEK_SET );

   /* TODO: Error handling .*/
   return 0;
}

int32_t drc_list_resources( const char* path, struct DRC_TOC_E** ptoc ) {
   FILE* drc_file = NULL;
   int32_t toc_count_out = 0,
      i = 0;
   struct DRC_TOC_E toc_e_iter;
   struct DRC_HEADER header;

   memset( &toc_e_iter, '\0', sizeof( struct DRC_TOC_E ) );

   dio_printf( "opening %s to list...\n", path );

   drc_file = fopen( path, "rb" );
   assert( NULL != drc_file );

   drc_read_header( drc_file, &header );

   assert( 0 != header.toc_start );
   for( i = 0 ; header.toc_entries > i ; i++ ) {

      assert( NULL == toc_e_iter.name );
      drc_read_toc_e( drc_file, &toc_e_iter );

      toc_count_out++;
      
      /* Store the entry in a list if requested. */
      if( NULL != ptoc ) {
         if( NULL == *ptoc ) {
            *ptoc = memory_alloc( 1, sizeof( struct DRC_TOC_E ) );
            assert( NULL != *ptoc );
         } else {
            *ptoc =
               realloc( *ptoc, sizeof( struct DRC_TOC_E ) * toc_count_out );
            assert( NULL != *ptoc );
         }

         /* Move, rather than copy. */
         memcpy( &((*ptoc)[i]), &toc_e_iter, sizeof( struct DRC_TOC_E ) );
         memset( &toc_e_iter, '\0', sizeof( struct DRC_TOC_E ) );
      } else {
         /* Just free the dynamically allocated name. */
         memory_free( &toc_e_iter.name );
         toc_e_iter.name = NULL;
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
   fwrite( &gc_drc_version, 1, sizeof( uint16_t ), drc_file ); /* Version */
   fwrite( &gc_zero_32, 1, sizeof( uint32_t ), drc_file );     /* CRC32 */
   fwrite( &file_sz, 1, sizeof( uint32_t ), drc_file );        /* Filesize */
   fwrite( &file_sz, 1, sizeof( uint32_t ), drc_file );     /* TOC Strt */
   fwrite( &gc_zero_32, 1, sizeof( uint32_t ), drc_file );     /* TOC Ct */
   fwrite( &gc_zero_32, 1, sizeof( uint32_t ), drc_file );     /* Reserved */
   fwrite( &file_sz, 1, sizeof( uint32_t ), drc_file );     /* FES */

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
      i = 0;
   uint32_t data_offset = 0,
      toc_iter_type = 0,
      toc_new_offset = 0,
      toc_iter_res = 0,
      toc_iter_id = 0,
      toc_iter_start = 0,
      toc_iter_size = 0,
      toc_iter_e_start_in = 0,   /* For integrity checking. */
      toc_iter_e_start_out = 0,  /* For integrity checking. */
      toc_entries = 0,
      old_sz = 0,
      new_sz = 0,
      read = 0,
      wrote = 0,
      read_total_data = 0;
   uint32_t* toc_offsets = NULL;
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

   /* Grab the current file size. */
   fseek( drc_file, 0, SEEK_END );
   old_sz = ftell( drc_file );

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
      dio_eprintf( "bad archive version\n" );
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
      dio_eprintf( "could not read from archive\n" );
      retval = DRC_ERROR_COULD_NOT_READ;
      goto cleanup;
   }
   if( DRC_HEADER_SZ != ftell( drc_file_tmp ) ) {
      dio_eprintf( "could not write to archive\n" );
      retval = DRC_ERROR_COULD_NOT_WRITE;
      goto cleanup;
   }

   /* Copy the TOC. */
   for( i = 0 ; toc_entries - 1 > i ; i++ ) {
      toc_iter_e_start_in = ftell( drc_file );
      toc_iter_e_start_out = ftell( drc_file_tmp );

      drc_copy( drc_file, drc_file_tmp, sizeof( uint32_t ), toc_iter_type );
      drc_copy( drc_file, drc_file_tmp, sizeof( uint32_t ), toc_iter_res );

      assert(
         ftell( drc_file ) == toc_iter_e_start_in + DRC_TOC_E_OFFSET_ID );
      assert(
         ftell( drc_file_tmp ) == toc_iter_e_start_out + DRC_TOC_E_OFFSET_ID );
      drc_copy( drc_file, drc_file_tmp, sizeof( uint32_t ), toc_iter_id );
      assert( 10000 > toc_iter_id );
      assert( 0 < toc_iter_id );

      assert( ftell( drc_file_tmp ) ==
         toc_iter_e_start_out + DRC_TOC_E_OFFSET_DATA_START );
      assert( ftell( drc_file ) ==
         toc_iter_e_start_in + DRC_TOC_E_OFFSET_DATA_START );
      
#if 0
      /* Add the offset of this entry's data_start member to the list to
       * rewrite later.
       */
      if( NULL == toc_offsets ) {
         toc_offsets_sz = 1;
         toc_offsets_new = memory_alloc( 1, sizeof( uint32_t ) );
      } else {
         toc_offsets_sz++;
         toc_offsets_new = realloc( toc_offsets, toc_offsets_sz );
      }
      assert( NULL != toc_offsets_new );
      if( NULL == toc_offsets_new ) {
         retval = DRC_ERROR_COULD_NOT_ALLOC;
         goto cleanup;
      }
      toc_offsets = toc_offsets_new;
      toc_offsets[toc_offsets_sz - 1] = ftell( drc_file_tmp );
      drc_copy( drc_file, drc_file_tmp, sizeof( uint32_t ), toc_iter_start );
#endif
      fread( &toc_iter_start, 1, sizeof( uint32_t ), drc_file );
      assert( toc_iter_start < old_sz );
      assert( 0 < toc_iter_start );
      dio_printf( "data start %u offset by %u bytes to %u\n",
         toc_iter_start, DRC_TOC_E_SZ + name_sz,
         toc_iter_start + DRC_TOC_E_SZ + name_sz );
      toc_iter_start += DRC_TOC_E_SZ + name_sz; /* Adjust data offset. */
      fwrite( &toc_iter_start, 1, sizeof( uint32_t ), drc_file_tmp );

      assert( ftell( drc_file_tmp ) ==
         toc_iter_e_start_out + DRC_TOC_E_OFFSET_DATA_SZ );
      assert( ftell( drc_file ) ==
         toc_iter_e_start_in + DRC_TOC_E_OFFSET_DATA_SZ );
      drc_copy( drc_file, drc_file_tmp, sizeof( uint32_t ), toc_iter_size );

      assert( ftell( drc_file_tmp ) ==
         toc_iter_e_start_out + DRC_TOC_E_OFFSET_NAME_SZ );
      assert( ftell( drc_file ) ==
         toc_iter_e_start_in + DRC_TOC_E_OFFSET_NAME_SZ );
      drc_copy( drc_file, drc_file_tmp, sizeof( uint16_t ), toc_iter_name_sz );

      dio_printf( "copied TOC entry (%ld bytes to %ld bytes)\n",
         ftell( drc_file ) - toc_iter_e_start_in,
         ftell( drc_file_tmp ) - toc_iter_e_start_out );
      assert( DRC_TOC_E_SZ == ftell( drc_file ) - toc_iter_e_start_in );
      assert( DRC_TOC_E_SZ ==
         ftell( drc_file_tmp ) - toc_iter_e_start_out );

      /* Copy the name. */
      toc_iter_name = memory_alloc( 1, toc_iter_name_sz );
      if( NULL == toc_iter_name ) {
         dio_eprintf( "could not allocate name buffer\n" );
         retval = DRC_ERROR_COULD_NOT_ALLOC;
         goto cleanup;
      }
      if( toc_iter_name_sz != fread(
         toc_iter_name, 1, toc_iter_name_sz, drc_file )
      ) {
         dio_eprintf( "could not read from archive\n" );
         retval = DRC_ERROR_COULD_NOT_READ;
         goto cleanup;
      }
      if( toc_iter_name_sz != fwrite(
         toc_iter_name, 1, toc_iter_name_sz, drc_file_tmp )
      ) {
         dio_eprintf( "could not write to archive\n" );
         retval = DRC_ERROR_COULD_NOT_WRITE;
         goto cleanup;
      }
      
      dio_printf( "skipping TOC entry %d, %s (starting at %u, %u bytes)..\n",
         toc_iter_id, toc_iter_name, toc_iter_start, toc_iter_size );

      memory_free( &toc_iter_name );
      toc_iter_name = NULL;
   }

   /* Write the new TOC entry. */
   dio_printf( "creating new TOC entry (%u) for file %u, %s (%u bytes)...\n",
      toc_entries - 1, id, name, buffer_sz );
   toc_new_offset = ftell( drc_file_tmp );
   wrote = fwrite( &type, 1, sizeof( uint32_t ), drc_file_tmp );
   wrote += fwrite( &gc_zero_8, 1, sizeof( uint32_t ), drc_file_tmp );
   assert( toc_new_offset + DRC_TOC_E_OFFSET_ID ==
      ftell( drc_file_tmp ) );
   fwrite( &id, 1, sizeof( uint32_t ), drc_file_tmp );
   fwrite( &gc_zero_32, 1, sizeof( uint32_t ), drc_file_tmp ); /* Come back. */
   fwrite( &buffer_sz, 1, sizeof( uint32_t ), drc_file_tmp );
   fwrite( &name_sz, 1, sizeof( uint16_t ), drc_file_tmp );
   fwrite( name, 1, name_sz, drc_file_tmp );

   /* Update the data offset. */
   data_offset = ftell( drc_file_tmp );
   dio_printf( "new data offset is %u...\n", data_offset );
   fseek( drc_file_tmp, DRC_HEADER_OFFSET_FES, SEEK_SET );
   fwrite( &data_offset, 1, sizeof( uint32_t ), drc_file_tmp );
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
   fseek( drc_file_tmp,
      toc_new_offset + DRC_TOC_E_OFFSET_DATA_START,
      SEEK_SET );
   fwrite( &data_offset, 1, sizeof( uint32_t ), drc_file_tmp );

   /* Append the new data. */
   fseek( drc_file_tmp, data_offset, SEEK_SET );
   assert( data_offset == ftell( drc_file_tmp ) );
   fwrite( buffer, 1, buffer_sz, drc_file_tmp );
   retval = data_offset;

   /* Update the filesize. */
   new_sz = ftell( drc_file_tmp );
   dio_printf( "new archive size is %u bytes\n", new_sz );
   fseek( drc_file_tmp, DRC_HEADER_OFFSET_FILESIZE, SEEK_SET );
   fwrite( &new_sz, 1, sizeof( uint32_t ), drc_file_tmp );

cleanup:

   if( NULL != toc_offsets ) {
      memory_free( &toc_offsets );
   }

   if( NULL != drc_file ) {
      fclose( drc_file );
   }

   if( NULL != drc_file_tmp ) {
      fclose( drc_file_tmp );
   }

   if( NULL != toc_iter_name ) {
      memory_free( &toc_iter_name );
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
   int32_t
      i = 0,
      resource_sz = 0,
      read = 0;
   struct DRC_TOC_E toc_e_iter;
   struct DRC_HEADER header;

   memset( &toc_e_iter, '\0', sizeof( struct DRC_TOC_E ) );

   dio_printf( "opening %s to get resource...\n", path );

   drc_file = fopen( path, "rb" );
   assert( NULL != drc_file );

   drc_read_header( drc_file, &header );
   assert( 0 != header.toc_start );
   for( i = 0 ; header.toc_entries > i ; i++ ) {
      assert( NULL == toc_e_iter.name );
      drc_read_toc_e( drc_file, &toc_e_iter );

      if( toc_e_iter.type == type && toc_e_iter.id == id ) {
         /* Resource found. */
         dio_printf( "found resource %u (at %u bytes, %u bytes long...\n",
            toc_e_iter.id, toc_e_iter.data_start, toc_e_iter.data_sz );
         resource_sz = toc_e_iter.data_sz;
         if( NULL == buffer ) {
            dio_eprintf( "no buffer ptr supplied\n" );
            goto cleanup;
         }
         /* Load file contents into buffer at referenced address. */
         *buffer = memory_alloc( 1, toc_e_iter.data_sz );
         dio_printf( "allocated %u bytes\n", toc_e_iter.data_sz );
         if( NULL == *buffer ) {
            dio_eprintf( "could not allocate contents buffer\n" );
            resource_sz = -1;
         } else {
            fseek( drc_file, toc_e_iter.data_start, SEEK_SET );
            read = fread( *buffer, 1, toc_e_iter.data_sz, drc_file );
            assert( read == toc_e_iter.data_sz );
            dio_printf( "read %d bytes from offset %u\n",
               read, toc_e_iter.data_start );
         }
         goto cleanup;
      } else {
         memory_free( &toc_e_iter.name );
         toc_e_iter.name = NULL;
      }
   }

cleanup:

   if( NULL != drc_file ) {
      fclose( drc_file );
   }

   if( NULL != toc_e_iter.name ) {
      memory_free( &toc_e_iter.name );
   }

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

   fread( &drc_file_sz, 1, sizeof( uint32_t ), drc_file );

   fclose( drc_file );

   return drc_file_sz;
}

int32_t drc_get_resource_sz( const char* path, uint32_t type, uint32_t id ) {
   FILE* drc_file = NULL;
   int32_t res_sz_out = 0,
      i = 0;
   struct DRC_TOC_E toc_e_iter;
   struct DRC_HEADER header;

   dio_printf( "opening %s to get resource size...\n", path );

   drc_file = fopen( path, "rb" );
   assert( NULL != drc_file );

   drc_read_header( drc_file, &header );

   assert( 0 != header.toc_start );
   for( i = 0 ; header.toc_entries > i ; i++ ) {

      assert( NULL == toc_e_iter.name );
      drc_read_toc_e( drc_file, &toc_e_iter );

      if( toc_e_iter.type == type && toc_e_iter.id == id ) {
         dio_printf( "found size for resource %u of type %s: %u bytes\n",
            toc_e_iter.id, (char*)&(toc_e_iter.type), toc_e_iter.data_sz );
         res_sz_out = toc_e_iter.data_sz;
         break;
      }

      memory_free( &toc_e_iter.name );
      toc_e_iter.name = NULL;
   }

   if( NULL != toc_e_iter.name ) {
      memory_free( &toc_e_iter.name );
   }
   fclose( drc_file );

   return res_sz_out;
}

int32_t drc_get_resource_name(
   const char* path, uint32_t type, uint32_t id, char** name_out
) {
   FILE* drc_file = NULL;
   int32_t name_sz_out = 0,
      i = 0;
   struct DRC_TOC_E toc_e_iter;
   struct DRC_HEADER header;

   dio_printf( "opening %s to get resource name...\n", path );

   drc_file = fopen( path, "rb" );
   assert( NULL != drc_file );

   drc_read_header( drc_file, &header );

   assert( 0 != header.toc_start );
   for( i = 0 ; header.toc_entries > i ; i++ ) {

      assert( NULL == toc_e_iter.name );
      drc_read_toc_e( drc_file, &toc_e_iter );
      assert( NULL != toc_e_iter.name );

      if( toc_e_iter.type == type && toc_e_iter.id == id ) {
         dio_printf( "found name for resource %u of type %s: %s\n",
            toc_e_iter.id, (char*)&(toc_e_iter.type), toc_e_iter.name );
         *name_out = toc_e_iter.name;
         name_sz_out = toc_e_iter.name_sz;
         break;
      }

      memory_free( &toc_e_iter.name );
   }

   if( 0 >= name_sz_out ) {
      memory_free( &toc_e_iter.name );
   }

   fclose( drc_file );

   return name_sz_out;
}

