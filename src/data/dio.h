
#ifndef DIO_H
#define DIO_H

#include "../dstypes.h"

#define DIO_ERROR_NULL_PTR             -1
#define DIO_ERROR_COPY_MISMATCH        -2
#define DIO_ERROR_COULD_NOT_OPEN_FILE  -3
#define DIO_ERROR_INVALID_STREAM       -4

#ifndef DIO_SILENT
#define dio_printf( ... ) printf( __VA_ARGS__ )
#define dio_eprintf( ... ) fprintf( stderr, __VA_ARGS__ )
#else
#ifdef NO_VARGS
#define dio_printf( x )
#define dio_eprintf( x )
#else
#define dio_printf( ... )
#define dio_eprintf( ... )
#endif /* NO_VARGS */
#endif

#define DIO_STREAM_BUFFER  1
#define DIO_STREAM_FILE    2

#include <stdio.h>

#define dio_assert_stream( stream ) \
   assert( \
      0 <= stream->buffer_sz && \
      0 <= stream->id && \
      ((DIO_STREAM_BUFFER == stream->type && NULL != stream->buffer.bytes ) || \
      (DIO_STREAM_FILE == stream->type && NULL != stream->buffer.file )) )

union DIO_BUFFER {
   FILE* file;
   uint8_t* bytes;
};

struct DIO_STREAM {
   int32_t id;
   uint8_t type;
   union DIO_BUFFER buffer;
   uint32_t buffer_sz;
   uint32_t position;
};

struct CONVERT_GRID;
struct DIO_STREAM;

int32_t dio_open_stream_file( const char*, const char*, struct DIO_STREAM* );
int32_t dio_open_stream_buffer( uint8_t*, uint32_t, struct DIO_STREAM* );
void dio_close_stream( struct DIO_STREAM* );
int32_t dio_seek_stream( struct DIO_STREAM*, int32_t, uint8_t );
int32_t dio_tell_stream( struct DIO_STREAM* );
int32_t dio_sz_stream( struct DIO_STREAM* );
int32_t dio_position_stream( struct DIO_STREAM* );
uint8_t dio_type_stream( struct DIO_STREAM* );
int32_t dio_read_stream( void*, uint32_t, struct DIO_STREAM* );
int32_t dio_write_stream( const void*, uint32_t, struct DIO_STREAM* );
uint32_t dio_reverse_endian_32( uint32_t );
uint16_t dio_reverse_endian_16( uint16_t );
int32_t dio_char_idx( const char*, int32_t, char );
int32_t dio_char_idx_r( const char*, int32_t, char );
int16_t dio_mktemp_path( char*, uint16_t, const char* );
int32_t dio_basename( const char*, uint32_t );
void dio_print_binary( uint8_t );
void dio_print_grid( struct CONVERT_GRID* );
int32_t dio_copy_file( const char*, const char* );
uint32_t dio_read_file( const char*, uint8_t** );
int32_t dio_move_file( const char*, const char* );
int16_t dio_itoa( char*, uint16_t, int16_t, uint8_t );
int16_t dio_atoi( const char*, int );
int16_t dio_strncmp( const char*, const char*, uint16_t, char );
int16_t dio_snprintf( char*, int, const char*, ... );

#endif /* DIO_H */

