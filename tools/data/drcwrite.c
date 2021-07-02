
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "drcwrite.h"
#include "../../src/data/drc.h"

#include "../../src/memory.h"

static const uint8_t gc_zero_8 = 0;
static const uint32_t gc_zero_32 = 0;
static const uint16_t gc_drc_version = 1;

#define drc_copy( src, dest, sz, buffer ) \
   assert( sizeof( buffer ) >= sz ); \
   if( sizeof( buffer ) < sz ) { \
      retval = DRC_ERROR_BAD_BUFFER; \
      goto cleanup; \
   } \
   if( sz != dio_read_stream( &buffer, sz, src ) ) { \
      assert( 1 == 0 ); \
      return DRC_ERROR_COULD_NOT_READ; \
   } \
   if( sz != dio_write_stream( &buffer, sz, dest ) ) { \
      assert( 1 == 0 ); \
      return DRC_ERROR_COULD_NOT_WRITE; \
   }

#define drc_change( src, dest, type, new_val ) \
   dio_seek_stream( src, sizeof( type ), SEEK_CUR ); \
   if( sizeof( type ) != dio_write_stream( &new_val, sizeof( type ), dest ) ) { \
      assert( 1 == 0 ); \
      return DRC_ERROR_COULD_NOT_WRITE; \
   }

#define drc_skip( src, dest, type ) \
   dio_seek_stream( src, sizeof( type ), SEEK_CUR ); \
   dio_seek_stream( dest, sizeof( type ), SEEK_CUR );

int32_t drc_create( struct DIO_STREAM* drc_file ) {
   struct DRC_HEADER header;

   assert( sizeof( union DRC_TYPE ) == 4 );

#if 0
   /* Set the TOC start and FES to just after the header.
    * They're both empty at this point.
    */
   dio_write_stream( "DRCT", sizeof( uint32_t ), drc_file );          /* Type */
   dio_write_stream( &gc_drc_version, sizeof( uint16_t ), drc_file ); /* Ver */
   dio_write_stream( &gc_zero_32, sizeof( uint32_t ), drc_file );     /* CRC */
   dio_write_stream( &file_sz, sizeof( uint32_t ), drc_file );        /* Size */
   dio_write_stream( &file_sz, sizeof( uint32_t ), drc_file );        /* TOCS */
   dio_write_stream( &gc_zero_32, sizeof( uint32_t ), drc_file );     /* TOCE */
   dio_write_stream( &gc_zero_32, sizeof( uint32_t ), drc_file );     /* Rsrv */
   dio_write_stream( &file_sz, sizeof( uint32_t ), drc_file );        /* FES */
#endif

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
      toc_iter_type = 0,
      toc_new_offset = 0,
      toc_iter_res = 0,
      toc_iter_id = 0,
      toc_iter_start = 0,
      toc_iter_size = 0,
      toc_iter_e_start_in = 0,   /* For integrity checking. */
      toc_iter_e_start_out = 0,  /* For integrity checking. */
      /* toc_entries = 0, */
      old_sz = 0,
      new_sz = 0,
      read = 0,
      wrote = 0,
      read_total_data = 0;
   uint32_t* toc_offsets = NULL;
   uint16_t toc_iter_name_sz = 0;
   uint8_t copy_buffer[DRC_COPY_BLOCK_SZ + 1];
   struct DRC_HEADER header_buffer;
   struct DRC_TOC_E toc_buffer;

   debug_printf( 2, "checking for existing resource..." );

   /* Make sure resource does not exist, get offset if it does. */
   existing_offset = drc_get_resource( drc_file_in, type, id, NULL, 0 );
   if( 0 > existing_offset && DRC_ERROR_COULD_NOT_FIND != existing_offset ) {
      /* Error occurred. */
      return existing_offset;
   } else if( 0 < existing_offset ) {
      /* Resource exists. */
      dio_printf( "resource %u/%u exists at %d; replacing...\n",
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

   /* Grab the current TOC count. */
   /* debug_printf( 2, "TOC entries is %d into drc header",
      offsetof( struct DRC_HEADER, toc_entries ) );
   dio_seek_stream(
      drc_file_in, offsetof( struct DRC_HEADER, toc_entries ), SEEK_SET );
   dio_read_stream( &toc_entries, sizeof( uint32_t ), drc_file_in ); */

   /* Get the size of the existing file. */
   /* data_offset = dio_sz_stream( drc_file_in ); */

   /* Reset source file to match dest starting point. */
 /*  dio_seek_stream( drc_file_in, 0, SEEK_SET ); */

   /* Copy the header. */
#if 0
   drc_copy( drc_file_in, drc_file_out, sizeof( uint32_t ),       /* Type */
      copy_buffer );
   drc_copy( drc_file_in, drc_file_out, sizeof( uint16_t ),       /* Version */
      copy_buffer );
   if( 1 != *((uint16_t*)&copy_buffer) ) {
      error_printf( "bad archive version" );
      retval = DRC_ERROR_BAD_VERSION;
      goto cleanup;
   }
   drc_change( drc_file_in, drc_file_out, uint32_t, gc_zero_32 ); /* CRC32 */
   drc_change( drc_file_in, drc_file_out, uint32_t, gc_zero_32 ); /* Filesize */
   drc_copy( drc_file_in, drc_file_out, sizeof( uint32_t ),       /* TOC Strt */
      copy_buffer );
   drc_change( drc_file_in, drc_file_out, uint32_t, toc_entries );/* TOC Ct */
   drc_copy( drc_file_in, drc_file_out, sizeof( uint32_t ),       /* Reserved */
      copy_buffer );
   drc_change( drc_file_in, drc_file_out, uint32_t, gc_zero_32 ); /* Dat Strt */
#endif
   /* drc_copy( drc_file_in, drc_file_out, sizeof( struct DRC_HEADER ),
      header_buffer ); */

   dio_read_stream( &header_buffer, sizeof( struct DRC_HEADER ), drc_file_in );

   debug_printf( 2, "existing archive is %d bytes w/ %u entries",
      header_buffer.filesize, header_buffer.toc_entries );

   /* We're adding one. */
   header_buffer.toc_entries++;
   header_buffer.filesize +=
      sizeof( struct DRC_TOC_E ) +     /* New TOC entry. */
      buffer_sz;                       /* New data. */
   header_buffer.first_entry_start += sizeof( struct DRC_TOC_E );

   if( sizeof( struct DRC_HEADER ) != dio_tell_stream( drc_file_in ) ) {
      error_printf( "read %d bytes; should have read %d bytes",
         dio_tell_stream( drc_file_in ), sizeof( struct DRC_HEADER ) );
      retval = DRC_ERROR_COULD_NOT_READ;
      goto cleanup;
   }

   /* Write the modified header. */
   dio_write_stream(
      &header_buffer, sizeof( struct DRC_HEADER ), drc_file_out );

   if( sizeof( struct DRC_HEADER ) != dio_tell_stream( drc_file_out ) ) {
      error_printf( "wrote %d bytes; should have written %d bytes",
         dio_tell_stream( drc_file_out ), sizeof( struct DRC_HEADER ) );
      retval = DRC_ERROR_COULD_NOT_WRITE;
      goto cleanup;
   }

   /* Copy the TOC. */
   for( i = 0 ; header_buffer.toc_entries - 1 > i ; i++ ) {
      toc_iter_e_start_in = dio_tell_stream( drc_file_in );
      toc_iter_e_start_out = dio_tell_stream( drc_file_out );

#if 0
      drc_copy( drc_file_in, drc_file_out, sizeof( uint32_t ), toc_iter_type );
      drc_copy( drc_file_in, drc_file_out, sizeof( uint32_t ), toc_iter_res );

      assert(
         dio_tell_stream( drc_file_in ) ==
            toc_iter_e_start_in + DRC_TOC_E_OFFSET_ID );
      assert(
         dio_tell_stream( drc_file_out ) ==
            toc_iter_e_start_out + DRC_TOC_E_OFFSET_ID );
      drc_copy( drc_file_in, drc_file_out, sizeof( uint32_t ), toc_iter_id );
#endif

      dio_read_stream( &toc_buffer, sizeof( struct DRC_TOC_E ), drc_file_in );
      drc_copy( drc_file_in, drc_file_out, sizeof( struct DRC_TOC_E ),
         toc_buffer );

      debug_printf( 2, "copying TOC entry %d...", toc_buffer.id );

      assert( 10000 > toc_buffer.id );
      assert( 0 < toc_buffer.id );

      /* Account for new TOC entry below. */
      toc_buffer.data_start += sizeof( struct DRC_TOC_E );

      dio_write_stream(
         &toc_buffer, sizeof( struct DRC_TOC_E ), drc_file_out );
#if 0
      assert( dio_tell_stream( drc_file_out ) ==
         toc_iter_e_start_out + DRC_TOC_E_OFFSET_DATA_START );
      assert( dio_tell_stream( drc_file_in ) ==
         toc_iter_e_start_in + DRC_TOC_E_OFFSET_DATA_START );
      
      dio_read_stream( &toc_iter_start, sizeof( uint32_t ), drc_file_in );
      assert( toc_iter_start < old_sz );
      assert( 0 < toc_iter_start );
      dio_printf( "data start %u offset by %u bytes to %u\n",
         toc_iter_start, DRC_TOC_E_SZ + name_sz,
         toc_iter_start + DRC_TOC_E_SZ + name_sz );
      toc_iter_start += DRC_TOC_E_SZ + name_sz; /* Adjust data offset. */
      dio_write_stream( &toc_iter_start, sizeof( uint32_t ), drc_file_out );

      assert( dio_tell_stream( drc_file_out ) ==
         toc_iter_e_start_out + DRC_TOC_E_OFFSET_DATA_SZ );
      assert( dio_tell_stream( drc_file_in ) ==
         toc_iter_e_start_in + DRC_TOC_E_OFFSET_DATA_SZ );
      drc_copy( drc_file_in, drc_file_out, sizeof( uint32_t ), toc_iter_size );

      assert( dio_tell_stream( drc_file_out ) ==
         toc_iter_e_start_out + DRC_TOC_E_OFFSET_NAME_SZ );
      assert( dio_tell_stream( drc_file_in ) ==
         toc_iter_e_start_in + DRC_TOC_E_OFFSET_NAME_SZ );
      drc_copy(
         drc_file_in, drc_file_out, sizeof( uint16_t ), toc_iter_name_sz );

      dio_printf( "copied TOC entry (%u bytes to %u bytes)\n",
         dio_tell_stream( drc_file_in ) - toc_iter_e_start_in,
         dio_tell_stream( drc_file_out ) - toc_iter_e_start_out );
      assert( DRC_TOC_E_SZ ==
         dio_tell_stream( drc_file_in ) - toc_iter_e_start_in );
      assert( DRC_TOC_E_SZ ==
         dio_tell_stream( drc_file_out ) - toc_iter_e_start_out );

      /* Copy the name. */
      toc_iter_name = memory_alloc( 1, toc_iter_name_sz );
      if( NULL == toc_iter_name ) {
         dio_eprintf( "could not allocate name buffer\n" );
         retval = DRC_ERROR_COULD_NOT_ALLOC;
         goto cleanup;
      }
      if( toc_iter_name_sz != dio_read_stream(
         toc_iter_name, toc_iter_name_sz, drc_file_in )
      ) {
         dio_eprintf( "could not read from archive\n" );
         retval = DRC_ERROR_COULD_NOT_READ;
         goto cleanup;
      }
      if( toc_iter_name_sz != dio_write_stream(
         toc_iter_name, toc_iter_name_sz, drc_file_out )
      ) {
         dio_eprintf( "could not write to archive\n" );
         retval = DRC_ERROR_COULD_NOT_WRITE;
         goto cleanup;
      }
      
      dio_printf( "skipping TOC entry %d, %s (starting at %u, %u bytes)..\n",
         toc_iter_id, toc_iter_name, toc_iter_start, toc_iter_size );

      memory_free( &toc_iter_name );
      toc_iter_name = NULL;
#endif
   }

   /* Store the offset of the new TOC entry for later. */
   toc_new_offset = dio_tell_stream( drc_file_out );

   /* Write the new TOC entry. */
   memset( &toc_buffer, '\0', sizeof( struct DRC_TOC_E ) );
   dio_printf( "creating new TOC entry (%u) for file %u, %s (%u bytes)...\n",
      header_buffer.toc_entries - 1, id, name, buffer_sz );
   toc_buffer.type.u32 = type.u32;
   toc_buffer.data_start =
      header_buffer.filesize +      /* Current end of the file. */
      sizeof( struct DRC_TOC_E );   /* This TOC entry we're writing now. */
   toc_buffer.data_sz = buffer_sz;
   toc_buffer.name_sz = name_sz;
   strncpy( toc_buffer.name, name, 64 );

   dio_write_stream( &toc_buffer, sizeof( struct DRC_TOC_E ), drc_file_out );

#if 0
   wrote = dio_write_stream( &type, sizeof( uint32_t ), drc_file_out );
   wrote += dio_write_stream( &gc_zero_8, sizeof( uint32_t ), drc_file_out );
   assert( toc_new_offset + DRC_TOC_E_OFFSET_ID ==
      dio_tell_stream( drc_file_out ) );
   dio_write_stream( &id, sizeof( uint32_t ), drc_file_out );
   /* Zero this out and come back later. */
   dio_write_stream( &gc_zero_32, sizeof( uint32_t ), drc_file_out );
   dio_write_stream( &buffer_sz, sizeof( uint32_t ), drc_file_out );
   dio_write_stream( &name_sz, sizeof( uint16_t ), drc_file_out );
   dio_write_stream( name, name_sz, drc_file_out );
#endif

#if 0
   /* Update the data offset. */
   data_offset = dio_tell_stream( drc_file_out );
   dio_printf( "new data offset is %u...\n", data_offset );
   dio_seek_stream(
      drc_file_out,
      offsetof( struct DRC_HEADER, first_entry_start ), SEEK_SET );
   dio_write_stream( &data_offset, sizeof( uint32_t ), drc_file_out );
   dio_seek_stream( drc_file_out, data_offset, SEEK_SET );

   /* Start copying data. */
   if( data_offset != dio_tell_stream( drc_file_out ) ) {
      retval = DRC_ERROR_COULD_NOT_WRITE;
      goto cleanup;
   }
#endif
   debug_printf( 2, "copying existing data..." );
   copy_remaining = header_buffer.filesize - header_buffer.first_entry_start;
   while( 0 < copy_remaining / DRC_COPY_BLOCK_SZ ) {
      dio_read_stream( copy_buffer, DRC_COPY_BLOCK_SZ, drc_file_in );
      dio_write_stream( copy_buffer, DRC_COPY_BLOCK_SZ, drc_file_out );
      copy_remaining -= DRC_COPY_BLOCK_SZ;
   }
   if( 0 < copy_remaining % DRC_COPY_BLOCK_SZ ) {
      dio_read_stream( copy_buffer, copy_remaining, drc_file_in );
      dio_write_stream( copy_buffer, copy_remaining, drc_file_out );
   }
#if 0
   while(
      0 != (read =
         dio_read_stream( copy_buffer, DRC_COPY_BLOCK_SZ, drc_file_in ))
   ) {
      read_total_data += read;
      dio_write_stream( copy_buffer, read, drc_file_out );
   }
   dio_printf( "copied %u bytes\n", read_total_data );
#endif

#if 0
   /* Update the TOC entry offset. */
   data_offset = dio_tell_stream( drc_file_out );
   dio_printf( "new entry's data offset is %u...\n", data_offset );
   dio_seek_stream( drc_file_out,
      toc_new_offset + DRC_TOC_E_OFFSET_DATA_START,
      SEEK_SET );
   dio_write_stream( &data_offset, sizeof( uint32_t ), drc_file_out );

#endif
   /* Append the new data. */
   /* dio_seek_stream( drc_file_out, data_offset, SEEK_SET );
   assert( data_offset == dio_tell_stream( drc_file_out ) ); */
   dio_write_stream( buffer, buffer_sz, drc_file_out );
   /* retval = data_offset; */

#if 0
   /* Update the filesize. */
   new_sz = dio_tell_stream( drc_file_out );
   dio_printf( "new archive size is %u bytes\n", new_sz );
   dio_seek_stream(
      drc_file_out, offsetof( struct DRC_HEADER, filesize ), SEEK_SET );
   dio_write_stream( &new_sz, sizeof( uint32_t ), drc_file_out );
#endif

cleanup:

   if( NULL != toc_offsets ) {
      memory_free( &toc_offsets );
   }

   /*
   if( NULL != drc_file_out ) {
      dio_close_stream( drc_file_out );
   }
   */

   if( NULL != toc_iter_name ) {
      memory_free( &toc_iter_name );
   }

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

