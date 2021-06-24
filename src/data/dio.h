
#ifndef DIO_H
#define DIO_H

#include "../dstypes.h"

#define DIO_ERROR_NULL_PTR             -1
#define DIO_ERROR_COPY_MISMATCH        -2
#define DIO_ERROR_COULD_NOT_OPEN_FILE  -3

#ifndef DIO_SILENT
#define dio_printf( ... ) printf( __VA_ARGS__ )
#define dio_eprintf( ... ) fprintf( stderr, __VA_ARGS__ )
#else
#define dio_printf( ... )
#define dio_eprintf( ... )
#endif

struct CONVERT_GRID;

uint32_t dio_reverse_endian_32( uint32_t );
uint16_t dio_reverse_endian_16( uint16_t );
int32_t dio_char_idx( const char*, int32_t, char );
int32_t dio_char_idx_r( const char*, int32_t, char );
int32_t dio_basename( const char*, uint32_t );
void dio_print_binary( uint8_t );
void dio_print_grid( struct CONVERT_GRID* );
int32_t dio_copy_file( const char*, const char* );
uint32_t dio_read_file( const char*, uint8_t** );
int32_t dio_move_file( const char*, const char* );
int16_t dio_itoa( char*, uint16_t, int16_t, uint8_t );
int16_t dio_snprintf( char*, uint16_t, const char*, ... );

#endif /* DIO_H */

