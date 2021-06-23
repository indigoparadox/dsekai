
#ifndef DIO_H
#define DIO_H

#include "../dstypes.h"

#ifndef DIO_SILENT
#define dio_printf( ... ) printf( __VA_ARGS__ )
#else
#define dio_printf( ... )
#endif

uint32_t dio_reverse_endian_32( uint32_t );
uint16_t dio_reverse_endian_16( uint16_t );
void dio_print_binary( uint8_t byte_in );
uint32_t dio_read_file( const char*, uint8_t** );

#endif /* DIO_H */

