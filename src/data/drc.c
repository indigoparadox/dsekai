
#include "drc.h"

#include "../memory.h"
#include "dio.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
   if( NULL == drc_file ) {
      return 0;
   }

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
