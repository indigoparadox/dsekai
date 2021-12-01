
#ifndef STRPOOL_H
#define STRPOOL_H

int16_t strpool_get_sz( const char* str_in );

/**
 * \param strpool
 */
void strpool_init( char* strpool, int16_t strpool_sz );

int16_t strpool_add_string( char* strpool, char* string, int16_t string_sz );

const char* strpool_get( const char* strpool, int16_t idx, int16_t* sz_out );

#endif /* !STRPOOL_H */

