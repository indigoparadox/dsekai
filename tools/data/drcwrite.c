
#include "../convert.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "drcwrite.h"

static const uint16_t gc_drc_version = 1;

#define drc_skip( src, dest, type ) \
   dio_seek_stream( src, sizeof( type ), SEEK_CUR ); \
   dio_seek_stream( dest, sizeof( type ), SEEK_CUR );

int32_t drc_create( struct DIO_STREAM* drc_file ) {
   struct DRC_HEADER header;

   assert( sizeof( union DRC_TYPE ) == 4 );

   header.type.str[0] = 'D';
   header.type.str[1] = 'R';
   header.type.str[2] = 'C';
   header.type.str[3] = 'T';
   header.version = gc_drc_version;
   header.crc32 = 0;
   header.filesize = sizeof( struct DRC_HEADER );
   header.toc_start = sizeof( struct DRC_HEADER );
   header.toc_entries = 0;
   header.first_entry_start = sizeof( struct DRC_HEADER );

   dio_write_stream( &header, sizeof( struct DRC_HEADER ), drc_file );

#ifndef CHECK
   assert( dio_tell_stream( drc_file ) == sizeof( struct DRC_HEADER ) );
#endif /* !CHECK */

   return sizeof( struct DRC_HEADER );
}

/**
 * @return Pointer to the newly created DRC, or NULL otherwise.
 */
int32_t drc_add_resource(
   struct DIO_STREAM* drc_file_in, struct DIO_STREAM* drc_file_out,
   union DRC_TYPE type, uint32_t id,
   const char* name, uint16_t name_sz, const uint8_t* buffer, uint32_t buffer_sz
) {
   int32_t retval = 0;
   char* toc_iter_name = NULL;
   int32_t existing_offset = 0,
      i = 0,
      copy_remaining = 0;
   uint32_t data_offset = 0,
      read = 0,
      wrote = 0,
      orig_filesize = 0,
      orig_first_entry_start = 0,
      read_total_data = 0;
   uint8_t copy_buffer[DRC_COPY_BLOCK_SZ + 1];
   struct DRC_HEADER header_buffer;
   struct DRC_TOC_E toc_buffer;

   /* Make sure resource does not exist, get offset if it does. */
   existing_offset = drc_get_resource( drc_file_in, type, id, NULL, 0 );
   if( 0 > existing_offset && DRC_ERROR_COULD_NOT_FIND != existing_offset ) {
      /* Error occurred. */
      return existing_offset;
   } else if( 0 < existing_offset ) {
      /* Resource exists. */
      debug_printf( 2, "resource %u/%u exists at %d; replacing...",
         type.u32, id, existing_offset );
      /* TODO: Rather than removing the resource, exclude it from the copy
               loop below. */
      /*existing_offset = drc_remove_resource( drc_file_in, type, id );*/
      if( 0 < existing_offset ) {
         /* Error occurred. */
         return existing_offset;
      }
   }

   dio_seek_stream( drc_file_in, 0, SEEK_SET );
   dio_seek_stream( drc_file_out, 0, SEEK_SET );

   /* Copy the header. */
   if(
      sizeof( struct DRC_HEADER ) != dio_read_stream(
         &header_buffer, sizeof( struct DRC_HEADER ), drc_file_in )
   ) {
      error_printf( "unable to read header" );
      return DRC_ERROR_COULD_NOT_READ;
   }
   debug_printf( 2, "existing archive is %d bytes w/ %u entries",
      header_buffer.filesize, header_buffer.toc_entries );

   /* We're adding one. */
   header_buffer.toc_entries++;

   /* Grab original filesizes for later before making adjustments. */
   orig_filesize = header_buffer.filesize;
   orig_first_entry_start = header_buffer.first_entry_start;
   header_buffer.filesize +=
      sizeof( struct DRC_TOC_E ) +     /* New TOC entry. */
      buffer_sz;                       /* New data. */
   header_buffer.first_entry_start += sizeof( struct DRC_TOC_E );

   if( sizeof( struct DRC_HEADER ) != dio_tell_stream( drc_file_in ) ) {
      error_printf( "read %d bytes; should have read %lu bytes",
         dio_tell_stream( drc_file_in ), sizeof( struct DRC_HEADER ) );
      retval = DRC_ERROR_COULD_NOT_READ;
      goto cleanup;
   }

   /* Write the modified header. */
   dio_write_stream(
      &header_buffer, sizeof( struct DRC_HEADER ), drc_file_out );

   if( sizeof( struct DRC_HEADER ) != dio_tell_stream( drc_file_out ) ) {
      error_printf( "wrote %d bytes; should have written %lu bytes",
         dio_tell_stream( drc_file_out ), sizeof( struct DRC_HEADER ) );
      retval = DRC_ERROR_COULD_NOT_WRITE;
      goto cleanup;
   }

   assert( dio_tell_stream( drc_file_out ) == sizeof( struct DRC_HEADER ) );
   assert( dio_tell_stream( drc_file_in ) == sizeof( struct DRC_HEADER ) );

   /* Copy the TOC. */
   assert( sizeof( struct DRC_HEADER ) == dio_tell_stream( drc_file_in ) );
   assert( sizeof( struct DRC_HEADER ) == dio_tell_stream( drc_file_out ) );
   assert( 0 < header_buffer.toc_entries );
   assert( 1024 > header_buffer.toc_entries );
   for( i = 0 ; header_buffer.toc_entries - 1 > i ; i++ ) {
      dio_read_stream( &toc_buffer, sizeof( struct DRC_TOC_E ), drc_file_in );

      debug_printf( 1, "copying TOC entry %d type %c%c%c%c (%s, %d bytes)...",
         toc_buffer.id, toc_buffer.type.str[0], toc_buffer.type.str[1],
         toc_buffer.type.str[2], toc_buffer.type.str[3],
         toc_buffer.name, toc_buffer.data_sz );

      assert( 10000 > toc_buffer.id );
      assert( 0 < toc_buffer.id );

      /* Account for new TOC entry below. */
      toc_buffer.data_start += sizeof( struct DRC_TOC_E );

      dio_write_stream(
         &toc_buffer, sizeof( struct DRC_TOC_E ), drc_file_out );
   }

   assert( dio_tell_stream( drc_file_out ) ==
      sizeof( struct DRC_HEADER ) +
      ((header_buffer.toc_entries - 1) * sizeof( struct DRC_TOC_E )) );
   assert( dio_tell_stream( drc_file_in ) == 
      header_buffer.first_entry_start - sizeof( struct DRC_TOC_E ) );

   /* Write the new TOC entry. */
   memset( &toc_buffer, '\0', sizeof( struct DRC_TOC_E ) );
   debug_printf( 2, "creating new TOC entry (%u) for file %u, %s (%u bytes)...",
      header_buffer.toc_entries - 1, id, name, buffer_sz );
   toc_buffer.type.u32 = type.u32;
   toc_buffer.id = id;
   toc_buffer.data_start =
      orig_filesize +      /* Current end of the file. */
      sizeof( struct DRC_TOC_E );   /* This TOC entry we're writing now. */
   toc_buffer.data_sz = buffer_sz;
   toc_buffer.name_sz = name_sz;
   strncpy( toc_buffer.name, name, 64 );

   dio_write_stream( &toc_buffer, sizeof( struct DRC_TOC_E ), drc_file_out );
   assert( dio_tell_stream( drc_file_out ) == header_buffer.first_entry_start );
   assert( dio_tell_stream( drc_file_in ) == 
      header_buffer.first_entry_start - sizeof( struct DRC_TOC_E ) );

   /* Copy the existing data. */
   copy_remaining = orig_filesize - orig_first_entry_start;
   debug_printf( 1, "copying remaining data (%d bytes)...",
      copy_remaining );
   while( 0 < copy_remaining / DRC_COPY_BLOCK_SZ ) {
      debug_printf( 1, "copying existing data block..." );
      dio_read_stream( copy_buffer, DRC_COPY_BLOCK_SZ, drc_file_in );
      dio_write_stream( copy_buffer, DRC_COPY_BLOCK_SZ, drc_file_out );
      copy_remaining -= DRC_COPY_BLOCK_SZ;
   }
   if( 0 < copy_remaining % DRC_COPY_BLOCK_SZ ) {
      debug_printf( 1, "copying final existing data block..." );
      dio_read_stream( copy_buffer, copy_remaining, drc_file_in );
      dio_write_stream( copy_buffer, copy_remaining, drc_file_out );
   }

   assert( dio_tell_stream( drc_file_in ) == orig_filesize );
   assert( dio_tell_stream( drc_file_out ) == toc_buffer.data_start );

   debug_printf( 1, "writing new data..." );
   dio_write_stream( buffer, buffer_sz, drc_file_out );

   assert( dio_tell_stream( drc_file_out ) == header_buffer.filesize );

cleanup:

   return retval;
}

int32_t drc_remove_resource(
   struct DIO_STREAM* drc_file_in, struct DIO_STREAM* drc_file_out,
   union DRC_TYPE type, uint32_t id
) {
   uint32_t removed_offset = 0;

   /* TODO */

   return removed_offset;
}

