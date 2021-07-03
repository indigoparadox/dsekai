
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
   uint32_t toc_e_start = 0,
      read = 0;

   if(
      sizeof( struct DRC_TOC_E ) !=
      dio_read_stream( toc_e, sizeof( struct DRC_TOC_E ), drc_file )
   ) {
      return -1;
   }


   /* TODO: Error handling .*/
   return sizeof( struct DRC_TOC_E );
}

static int32_t drc_read_header(
   struct DIO_STREAM* drc_file, struct DRC_HEADER* h
) {
   dio_seek_stream( drc_file, 0, SEEK_SET );
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

   dio_seek_stream( drc_file, header.toc_start, SEEK_SET );

   assert( 0 != header.toc_start );
   for( i = 0 ; header.toc_entries > i ; i++ ) {

      drc_read_toc_e( drc_file, &toc_e_iter );

      toc_count_out++;

      /* Store the entry in a list if requested. */
      if( NULL != ptoc ) {
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

         if( NULL != *ptoc && (0 == ptoc_sz || i < ptoc_sz - 1) ) {
            debug_printf( 1, "copying listing result for TOC entry %d",
               toc_e_iter.id );
            /* Move, rather than copy. */
            memcpy( &((*ptoc)[i]), &toc_e_iter, sizeof( struct DRC_TOC_E ) );
            memset( &toc_e_iter, '\0', sizeof( struct DRC_TOC_E ) );
         }
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
      drc_read_toc_e( drc_file, &toc_e_iter );

      if( toc_e_iter.type.u32 != type.u32 && toc_e_iter.id != id ) {
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

