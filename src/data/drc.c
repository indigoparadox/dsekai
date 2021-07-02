
/**
 * \file
 */

#include "drc.h"

#include "../memory.h"
#include "dio.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * \brief Read a struct DRC_TOC_E from the provided DRC archive at its current
 *        offset.
 *
 * @param drc_file Pointer to the archive file being read.
 * @param toc_e Pointer to a struct DRC_TOC_E to read into.
 * @return
 */
static int32_t drc_read_toc_e(
   struct DIO_STREAM* drc_file, struct DRC_TOC_E* toc_e
) {
   uint32_t toc_e_start = 0;

   dio_read_stream( toc_e, sizeof( struct DRC_TOC_E ), drc_file );

#if 0
   assert( NULL == toc_e->name );
   if( NULL != toc_e->name ) {
      memory_free( &toc_e->name );
      toc_e->name = NULL;
   }

   toc_e_start = dio_tell_stream( drc_file );
   memset( toc_e, '\0', sizeof( struct DRC_TOC_E ) );

   dio_read_stream( &(toc_e->type), sizeof( uint32_t ), drc_file );
   dio_read_stream( &(toc_e->reserved), sizeof( uint32_t ), drc_file );
   
   assert( dio_tell_stream( drc_file ) == toc_e_start + DRC_TOC_E_OFFSET_ID );
   dio_read_stream( &(toc_e->id), sizeof( uint32_t ), drc_file );
   assert( 10000 > toc_e->id );

   assert( dio_tell_stream( drc_file ) ==
      toc_e_start + DRC_TOC_E_OFFSET_DATA_START );
   dio_read_stream( &(toc_e->data_start), sizeof( uint32_t ), drc_file );
   
   assert( dio_tell_stream( drc_file ) == 
      toc_e_start + DRC_TOC_E_OFFSET_DATA_SZ );
   dio_read_stream( &(toc_e->data_sz), sizeof( uint32_t ), drc_file );
   
   assert( dio_tell_stream( drc_file ) ==
      toc_e_start + DRC_TOC_E_OFFSET_NAME_SZ );
   dio_read_stream( &(toc_e->name_sz), sizeof( uint16_t ), drc_file );

#ifdef MEMORY_STATIC
   /* Skip the name, since we can't allocate for it. */
   toc_e->name = NULL;
   if( 0 > dio_seek_stream( drc_file, toc_e->name_sz, SEEK_CUR ) ) {
      return -1;
   }
   toc_e->name_sz = 0;
#else
   assert( 0 < toc_e->name_sz );
   toc_e->name = memory_alloc( toc_e->name_sz + 1, 1 );
   assert( NULL != toc_e->name );

   debug_printf( 1, "read TOC entry (%u bytes)",
      dio_tell_stream( drc_file ) - toc_e_start );
   assert( DRC_TOC_E_SZ == dio_tell_stream( drc_file ) - toc_e_start );

   dio_read_stream( toc_e->name, toc_e->name_sz, drc_file );
#endif /* MEMORY_STATIC */
   
   debug_printf( 1, "%u: %s (%s, starts at %u bytes, %u bytes long)",
      toc_e->id, toc_e->name, (char*)&(toc_e->type),
      toc_e->data_start, toc_e->data_sz );

   assert( dio_tell_stream( drc_file ) ==
      toc_e_start + DRC_TOC_E_SZ + toc_e->name_sz );

#endif

   /* TODO: Error handling .*/
   return 0;
}

static int32_t drc_read_header(
   struct DIO_STREAM* drc_file, struct DRC_HEADER* h
) {

#if 0
   dio_read_stream( &h->type, sizeof( uint32_t ), drc_file );
   dio_read_stream( &h->version, sizeof( uint16_t ), drc_file );
#ifndef CHECK
   assert( 1 == h->version );
#endif /* !CHECK */
   debug_printf( 2, "filesize is %d into drc header",
      offsetof( struct DRC_HEADER, filesize ) );
   if( 0 > dio_seek_stream(
      drc_file, offsetof( struct DRC_HEADER, filesize ), SEEK_SET )
   ) {
      return -1;
   }
   dio_read_stream( &h->filesize, sizeof( uint32_t ), drc_file );
   dio_read_stream( &h->toc_start, sizeof( uint32_t ), drc_file );
   dio_read_stream( &h->toc_entries, sizeof( uint32_t ), drc_file );

#ifndef CHECK
   assert( dio_sz_stream( drc_file ) == h->filesize );
#endif /* !CHECK */

   if( 0 > dio_seek_stream( drc_file, h->toc_start, SEEK_SET ) ) {
      return -1;
   }
#endif

   dio_read_stream( h, sizeof( struct DRC_HEADER ), drc_file );
   if( 0 > dio_seek_stream(
      drc_file, offsetof( struct DRC_HEADER, filesize ), SEEK_SET )
   ) {
      error_printf( "unable to read drc header" );
      return -1;
   }

   return 0;
}

/**
 * \brief List the resources available in the DRC archive at the given path.
 * \param path
 * \param ptoc Pointer to a list of struct DRC_TOC_E objects to fill with
 *             the resulting entries. If the targeted pointer is NULL, and
 *             dynamic memory is enabled, then it will be allocated.
 * \param ptoc_sz
 * \return
 */
int32_t drc_list_resources(
   struct DIO_STREAM* drc_file, struct DRC_TOC_E** ptoc, uint16_t ptoc_sz
) {
   int32_t toc_count_out = 0,
      i = 0;
   struct DRC_TOC_E toc_e_iter;
   struct DRC_HEADER header;
   struct DRC_TOC_E* toc_e_new_ptr = NULL;

   memset( &toc_e_iter, '\0', sizeof( struct DRC_TOC_E ) );

   debug_printf( 2, "opening drc to list..." );

   drc_read_header( drc_file, &header );

   assert( 0 != header.toc_start );
   for( i = 0 ; header.toc_entries > i ; i++ ) {

      assert( NULL == toc_e_iter.name );
      drc_read_toc_e( drc_file, &toc_e_iter );

      toc_count_out++;

      /* Store the entry in a list if requested. */
      if( NULL != ptoc ) {
#if 0
#ifndef MEMORY_STATIC
         if( NULL == *ptoc ) {
            *ptoc = memory_alloc(
               DRC_TOC_INITIAL_ALLOC, sizeof( struct DRC_TOC_E ) );
            assert( NULL != *ptoc );
         } else if( 0 == ptoc_sz || i >= ptoc_sz ) {
            toc_e_new_ptr = memory_realloc(
               *ptoc, sizeof( struct DRC_TOC_E ) * toc_count_out );
            if( NULL != toc_e_new_ptr ) {
               *ptoc = toc_e_new_ptr;
            } else {
               goto cleanup;
            }
         }
#endif /* !MEMORY_STATIC */
#endif
         if( NULL != *ptoc && (0 == ptoc_sz || i < ptoc_sz - 1) ) {
            /* Move, rather than copy. */
            memcpy( &((*ptoc)[i]), &toc_e_iter, sizeof( struct DRC_TOC_E ) );
            memset( &toc_e_iter, '\0', sizeof( struct DRC_TOC_E ) );
         }
#ifndef MEMORY_STATIC
      } else {
#if 0
         /* Just free the dynamically allocated name. */
         memory_free( &toc_e_iter.name );
         toc_e_iter.name = NULL;
#endif
#endif /* !MEMORY_STATIC */
      }
   }

cleanup:

   return toc_count_out;
}

int32_t drc_get_resource_info(
   struct DIO_STREAM* drc_file,
   union DRC_TYPE type, uint32_t id, struct DRC_TOC_E* e
) {
   int32_t i = 0;
   struct DRC_HEADER header;
   struct DRC_TOC_E toc_e_iter;

   debug_printf( 2, "opening drc to get resource info..." );

   assert( NULL != e );

   drc_read_header( drc_file, &header );
#ifndef CHECK
   assert( 0 != header.toc_start );
#endif /* !CHECK */
   debug_printf( 2, "drc is %d bytes long; found %d TOC entries",
      header.filesize, header.toc_entries );
   for( i = 0 ; header.toc_entries > i ; i++ ) {
      assert( NULL == toc_e_iter.name );
      drc_read_toc_e( drc_file, &toc_e_iter );

      if( toc_e_iter.type.u32 != type.u32 && toc_e_iter.id == id ) {
#if 0
#ifndef MEMORY_STATIC
         memory_free( &toc_e_iter.name );
#endif /* !MEMORY_STATIC */
#endif
         continue;
      }

      /* Resource found. */

      debug_printf( 2,
         "found resource %u of type %c%c%c%c (at %u bytes, %u bytes long...",
         toc_e_iter.id, toc_e_iter.type.str[0], toc_e_iter.type.str[1],
         toc_e_iter.type.str[2], toc_e_iter.type.str[3],
         toc_e_iter.data_start, toc_e_iter.data_sz );

      memcpy( e, &toc_e_iter, sizeof( struct DRC_TOC_E ) );

      goto cleanup;
   }

   i = DRC_ERROR_COULD_NOT_FIND;

cleanup:

   return i;
}

/**
 * \param buffer Pointer to a pointer to a buffer to fill with the resource
 *               requested, or pointer to a NULL pointer to allocate this
 *               buffer dynamically.
 * \param buffer_sz Allocated size of the buffer provided, or 0 if none.
 * \return Size (in bytes) of resource if found, or 0 or less otherwise.
 */
int32_t drc_get_resource(
   struct DIO_STREAM* drc_file, union DRC_TYPE type, uint32_t id,
   uint8_t* buffer, uint16_t buffer_sz
) {
   int32_t
      resource_sz = 0,
      read = 0;
   struct DRC_TOC_E toc_e_iter;

   memset( &toc_e_iter, '\0', sizeof( struct DRC_TOC_E ) );

   debug_printf( 2, "opening drc to get resource..." );

   read = drc_get_resource_info( drc_file, type, id, &toc_e_iter );
   if( 0 >= read ) {
      error_printf( "could not find resource %d type %c%c%c%c",
         id, type.str[0], type.str[1], type.str[2], type.str[3] );
      return read;
   }

   resource_sz = toc_e_iter.data_sz;
   if( NULL == buffer ) {
      error_printf( "no buffer ptr supplied" );
      /* Don't assign an error value, since no pointer is valid.
         * Just return the requested size. 
         */
      goto cleanup;
   }

   /* Load file contents into buffer at referenced address. */
   if( 0 > dio_seek_stream( drc_file, toc_e_iter.data_start, SEEK_SET ) ) {
      resource_sz = DRC_ERROR_COULD_NOT_READ;
      goto cleanup;
   }
   read = dio_read_stream( buffer, toc_e_iter.data_sz, drc_file );
   if( toc_e_iter.data_sz != read ) {
      error_printf( "problem reading data from TOC entry" );
      resource_sz = DRC_ERROR_COULD_NOT_READ;
      goto cleanup;
   }
   debug_printf( 2, "read %d bytes from offset %u",
      read, toc_e_iter.data_start );

cleanup:

#ifndef MEMORY_STATIC
#if 0
   if( NULL != toc_e_iter.name ) {
      memory_free( &toc_e_iter.name );
   }
#endif
#endif /* !MEMORY_STATIC */

   return resource_sz;
}

int32_t drc_get_resource_sz(
   struct DIO_STREAM* drc_file, union DRC_TYPE type, uint32_t id
) {
   int32_t res_sz_out = 0,
      i = 0;
   struct DRC_TOC_E toc_e_iter;
   struct DRC_HEADER header;

   debug_printf( 2, "opening drc to get resource size..." );

   drc_read_header( drc_file, &header );

   assert( 0 != header.toc_start );
   for( i = 0 ; header.toc_entries > i ; i++ ) {

      assert( NULL == toc_e_iter.name );
      drc_read_toc_e( drc_file, &toc_e_iter );

      if( toc_e_iter.type.u32 == type.u32 && toc_e_iter.id == id ) {
         debug_printf( 2, "found size for resource %u of type %s: %u bytes",
            toc_e_iter.id, (char*)&(toc_e_iter.type), toc_e_iter.data_sz );
         res_sz_out = toc_e_iter.data_sz;
         break;
      }

      /*
      memory_free( &toc_e_iter.name );
      toc_e_iter.name = NULL;
      */
   }

#if 0
#ifndef MEMORY_STATIC
   if( NULL != toc_e_iter.name ) {
      memory_free( &toc_e_iter.name );
   }
#endif /* MEMORY_STATIC */
#endif

   return res_sz_out;
}

int32_t drc_get_resource_name(
   struct DIO_STREAM* drc_file, union DRC_TYPE type, uint32_t id,
   char** name_out
) {
   int32_t name_sz_out = 0,
      i = 0;
   struct DRC_TOC_E toc_e_iter;
   struct DRC_HEADER header;

   debug_printf( 2, "opening drc to get resource name..." );

   drc_read_header( drc_file, &header );

   assert( 0 != header.toc_start );
   for( i = 0 ; header.toc_entries > i ; i++ ) {

      assert( NULL == toc_e_iter.name );
      drc_read_toc_e( drc_file, &toc_e_iter );
      assert( NULL != toc_e_iter.name );

      if( toc_e_iter.type.u32 == type.u32 && toc_e_iter.id == id ) {
         debug_printf( 2, "found name for resource %u of type %s: %s",
            toc_e_iter.id, (char*)&(toc_e_iter.type), toc_e_iter.name );
         *name_out = toc_e_iter.name;
         name_sz_out = toc_e_iter.name_sz;
         break;
      }

#if 0
      memory_free( &toc_e_iter.name );
#endif
   }

#if 0
   if( 0 >= name_sz_out ) {
      memory_free( &toc_e_iter.name );
   }
#endif

   return name_sz_out;
}

