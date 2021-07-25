
/**
 * \file
 */

#include "unilayer.h"

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
   if(
      sizeof( struct DRC_TOC_E ) !=
      dio_read_stream( (MEMORY_PTR)toc_e, sizeof( struct DRC_TOC_E ), drc_file )
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
   dio_read_stream( (MEMORY_PTR)h, sizeof( struct DRC_HEADER ), drc_file );
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
 * \param htoc Pointer to a list of struct DRC_TOC_E objects to fill with
 *             the resulting entries. If the targeted pointer is NULL, and
 *             dynamic memory is enabled, then it will be allocated.
 * \param htoc_sz
 * \return
 */
int32_t drc_list_resources(
   struct DIO_STREAM* drc_file, MEMORY_HANDLE* htoc, uint16_t htoc_sz
) {
   int32_t toc_count_out = 0,
      i = 0,
      new_toc_list_sz = 0;
   struct DRC_TOC_E toc_e_iter;
   struct DRC_HEADER header;
   struct DRC_TOC_E* toc_ptr = NULL;

   memory_zero_ptr( (MEMORY_PTR)&toc_e_iter, sizeof( struct DRC_TOC_E ) );

   debug_printf( 2, "opening drc to list..." );

   drc_read_header( drc_file, &header );
   dio_seek_stream( drc_file, header.toc_start, SEEK_SET );

   assert( header.toc_start == sizeof( struct DRC_HEADER ) );
   for( i = 0 ; header.toc_entries > i ; i++ ) {

      drc_read_toc_e( drc_file, &toc_e_iter );
 
      assert( header.toc_start + ((i + 1) * sizeof( struct DRC_TOC_E )) ==
         dio_tell_stream( drc_file ) );

      toc_count_out++;

      /* Store the entry in a list if requested. */
      if( NULL != htoc ) {
         if( NULL == *htoc ) {
            *htoc = memory_alloc(
               DRC_TOC_INITIAL_ALLOC, sizeof( struct DRC_TOC_E ) );
         } else if( 0 == htoc_sz || i >= htoc_sz ) {
            new_toc_list_sz = sizeof( struct DRC_TOC_E ) * toc_count_out;
            if( memory_resize( *htoc, new_toc_list_sz ) < new_toc_list_sz ) {
               error_printf( "unable to allocate new TOC list" );
               toc_count_out--;
               goto cleanup;
            }
         }
         assert( NULL != *htoc );

         toc_ptr = (struct DRC_TOC_E*)memory_lock( *htoc );
         debug_printf( 1, "copying listing result for TOC entry %d",
            toc_e_iter.id );
         /* Move, rather than copy. */
         memory_copy_ptr(
            (MEMORY_PTR)&(toc_ptr[i]), (MEMORY_PTR)&toc_e_iter,
            sizeof( struct DRC_TOC_E ) );
         assert( toc_ptr[i].type.u32 == toc_e_iter.type.u32 );
         memory_zero_ptr( (MEMORY_PTR)&toc_e_iter, sizeof( struct DRC_TOC_E ) );
         toc_ptr = (struct DRC_TOC_E*)memory_unlock( *htoc );
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
   union DRC_TYPE header_type = DRC_ARCHIVE_TYPE;
   struct DRC_HEADER header;
   uint8_t* header_bytes = (uint8_t*)&header;
   struct DRC_TOC_E toc_e_iter;

   debug_printf( 2, "opening drc to get resource info..." );

   assert( NULL != e );

   drc_read_header( drc_file, &header );
   if( 
      header_type.u32 != header.type.u32 ||
      header.toc_start == 0 ||
      header.toc_start > dio_sz_stream( drc_file ) ||
      header.first_entry_start == 0 ||
      header.first_entry_start > dio_sz_stream( drc_file ) ||
      header.first_entry_start < header.toc_start ||
      header.toc_entries >= DRC_MAX_ENTRIES
   ) {
      error_printf( "invalid archive header" );
      error_printf( "header is %lu bytes, looks like: ",
         sizeof( struct DRC_HEADER ) );
      for( i = 0 ; sizeof( struct DRC_HEADER ) > i ; i++ ) {
         error_printf( "%02x", header_bytes[i] );
      }
      error_printf( "header fields:" );
      error_printf( "   type: %c%c%c%c",
         header.type.str[0], header.type.str[1], header.type.str[2],
         header.type.str[3] );
      error_printf( "   version: %u", header.version );
      error_printf( "   crc32: %u", header.crc32 );
      error_printf( "   filesize: %u", header.filesize );
      error_printf( "   toc_start: %u", header.toc_start );
      error_printf( "   toc_entries: %u", header.toc_entries );
      error_printf( "   reserved: %u", header.reserved );
      error_printf( "   first_entry_start: %u", header.first_entry_start );
      return -1;
   }
   debug_printf( 2, "drc is %u bytes long; found %u TOC entries",
      header.filesize, header.toc_entries );
   dio_seek_stream( drc_file, header.toc_start, SEEK_SET );
   for( i = 0 ; header.toc_entries > i ; i++ ) {
      drc_read_toc_e( drc_file, &toc_e_iter );

      debug_printf( 2, "comparing: %c%c%c%c vs %c%c%c%c, %d vs %d",
         toc_e_iter.type.str[0], toc_e_iter.type.str[1],
         toc_e_iter.type.str[2], toc_e_iter.type.str[3],
         type.str[0], type.str[1], type.str[2], type.str[3],
         toc_e_iter.id, id );

      if( toc_e_iter.type.u32 != type.u32 || toc_e_iter.id != id ) {
         continue;
      }

      /* Resource found. */

      debug_printf( 2,
         "found resource %u of type %c%c%c%c (at %u bytes, %u bytes long...",
         toc_e_iter.id, toc_e_iter.type.str[0], toc_e_iter.type.str[1],
         toc_e_iter.type.str[2], toc_e_iter.type.str[3],
         toc_e_iter.data_start, toc_e_iter.data_sz );

      memory_copy_ptr(
         (MEMORY_PTR)e, (MEMORY_PTR)&toc_e_iter, sizeof( struct DRC_TOC_E ) );

      i = sizeof( struct DRC_TOC_E );

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
   MEMORY_PTR buffer, uint16_t buffer_sz
) {
   int32_t
      resource_sz = 0,
      read = 0;
   struct DRC_TOC_E toc_e_iter;

   memory_zero_ptr( (MEMORY_PTR)&toc_e_iter, sizeof( struct DRC_TOC_E ) );

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
      error_printf( "unable to seek to resource location in archive" );
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

   return resource_sz;
}

int32_t drc_get_resource_sz(
   struct DIO_STREAM* drc_file, union DRC_TYPE type, uint32_t id
) {
   int32_t res_sz_out = 0;
   struct DRC_TOC_E toc_e_iter;

   res_sz_out = drc_get_resource_info( drc_file, type, id, &toc_e_iter );
   if( 0 >= res_sz_out ) {
      return res_sz_out;
   }

   return toc_e_iter.data_sz;
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

int32_t drc_get_resource_id_by_name(
   struct DIO_STREAM* drc_file, union DRC_TYPE* type_out,
   const char* name, uint16_t name_sz
) {
   int32_t
      id_out = -1,
      i = 0;
   struct DRC_TOC_E toc_e_iter;
   MEMORY_HANDLE toc_handle = NULL;
   struct DRC_TOC_E* toc = NULL;
   int16_t toc_sz = 0;

   memory_zero_ptr( (MEMORY_PTR)&toc_e_iter, sizeof( struct DRC_TOC_E ) );

   debug_printf( 2, "opening drc to get resource by name..." );

   toc_sz = drc_list_resources( drc_file, &toc_handle, 0 );
   if( NULL == toc_handle ) {
      error_printf( "could not get DRC TOC" );
      return DRC_ERROR_COULD_NOT_FIND;
   }
   toc = (struct DRC_TOC_E*)memory_lock( toc_handle );
   for( i = 0 ; toc_sz > i ; i++ ) {
      if( 0 == dio_strncmp( toc[i].name, name, name_sz, '\0' ) ) {
         id_out = toc[i].id;
         type_out->u32 = toc[i].type.u32;
         break;
      }
   }
   toc = (struct DRC_TOC_E*)memory_unlock( toc_handle );
   memory_free( toc_handle );

   return id_out;
}

