
#ifndef STRPOOL_H
#define STRPOOL_H

/**
 * \addtogroup dsekai_strpool String Pool
 * \brief Flexible storage for strings in small spaces.
 *
 * String pools are used so that all of the strings that may be used by the UI
 * are kept in a spot that can be easily referenced even when the OS moves
 * memory around. Their flexible nature conserves precious memory.
 *
 * | 0x0          | 0x2               | 0x4                   | ...
 * |--------------|-------------------|-----------------------|-----
 * | strpool size | first string size | first string contents | ...
 *
 * All size values are stored LSB first, so that non-zero values are more
 * immediately apparent.
 *
 * There is an initial 16-bit value denoting the size of the entire pool. This
 * can be considered the pool header and does not repeat.
 * 
 * This is followed by a 16-bit value denoting the size of the first string,
 * followed by the string, itself (NULL-terminated; the sizes do not include
 * this termination). This size-contents-terminator pattern repeats for the
 * rest of the pool.
 *
 * \{
 */

/*! \file strpool.h
 *  \brief Flexible storage for strings in small spaces.
 */

/**
 * \brief Get the size of a string pool or contained string.
 * \param str_in String pool or character offset of string size to read.
 * \return Size of pool or string.
 */
int16_t strpool_get_sz( const char* str_in );

/**
 * \brief Prepare a string pool for use.
 * \param strpool String pool to initialize (zero out and imprint size).
 * \param strpool_sz Size allocated to the string pool (including all headers).
 */
void strpool_init( char* strpool, int16_t strpool_sz );

int16_t strpool_add_string( char* strpool, char* string, int16_t string_sz );

const char* strpool_get( const char* strpool, int16_t idx, int16_t* sz_out );

/* \} */

#endif /* !STRPOOL_H */

