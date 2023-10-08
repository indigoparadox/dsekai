
#ifndef SERIAL_H
#define SERIAL_H

/**
 * \addtogroup dsekai_engines
 * \{
 */

/**
 * \file serial.h
 * \brief Functions for (de)serializing engine structs to save states.
 */

#define SERIAL_ERROR -1

#define serial_asn_write_seq_start( p_asn_buffer_h, p_mark, desc, idx, cleanup ) debug_printf( 2, "(offset 0x%02x) writing %s seq start", idx, desc ); idx = asn_write_seq_start( p_asn_buffer_h, idx, p_mark ); if( 0 > idx ) { error_printf( "error writing %s seq start", desc ); idx = SERIAL_ERROR; goto cleanup; }

#define serial_asn_write_seq_end( p_asn_buffer_h, p_mark, desc, idx, cleanup ) debug_printf( 2, "(offset 0x%02x) writing %s seq end", idx, desc ); idx = asn_write_seq_end( p_asn_buffer_h, idx, p_mark ); if( 0 > idx ) { error_printf( "error writing %s seq end", desc ); idx = SERIAL_ERROR; goto cleanup; }

#define serial_asn_write_int( p_asn_buffer_h, val, x_val_sz, desc, idx, cleanup ) debug_printf( 2, "(offset 0x%02x) writing %s: %d", idx, desc, val ); idx = asn_write_int( p_asn_buffer_h, idx, val ); if( 0 > idx ) { error_printf( "error writing %s: %d", desc, idx ); goto cleanup; }

#define serial_asn_write_string( p_asn_buffer_h, str, str_sz, desc, idx, cleanup ) debug_printf( 2, "(offset 0x%02x) writing %s: %s", idx, desc, str ); idx = asn_write_string( p_asn_buffer_h, idx, str, str_sz ); if( 0 > idx ) { error_printf( "error writing %s: %d", desc, idx ); goto cleanup; }

#define serial_asn_read_seq( asn_buffer, p_type, p_sz, desc, idx, read_sz, cleanup ) read_sz = asn_read_meta_ptr( asn_buffer, idx, p_type, p_sz ); if( ASN_SEQUENCE != *(p_type) ) { error_printf( "invalid %s sequence type: 0x%02x", desc, *(p_type) ); idx = SERIAL_ERROR; goto cleanup; } debug_printf( 2, "(offset 0x%02x) read %s seq start (%d bytes)", idx, desc, *(p_sz) ); idx += read_sz;

#define serial_asn_read_int( asn_buffer, p_val, val_sz, flags, desc, idx, read_sz, cleanup ) read_sz = asn_read_int( (uint8_t*)(p_val), val_sz, flags, asn_buffer, idx ); if( 0 >= read_sz ) { error_printf( "error reading %s", desc ); idx = SERIAL_ERROR; goto cleanup; } debug_printf( 2, "(offset 0x%02x) read %s (%d bytes): %d", idx, desc, read_sz, *(p_val) ); idx += read_sz;

#define serial_asn_read_string( asn_buffer, str_buffer, str_buffer_sz, desc, idx, read_sz, cleanup ) read_sz = asn_read_string( str_buffer, str_buffer_sz, asn_buffer, idx ); if( 0 >= read_sz ) { error_printf( "error reading %s", desc ); idx = SERIAL_ERROR; goto cleanup; } debug_printf( 2, "(offset 0x%02x) read %s (%d bytes): %s", idx, desc, read_sz, str_buffer ); idx += read_sz;

/* int16_t engines_asn_load( RESOURCE_ID id, struct DSEKAI_STATE* state ); */

/**
 * \brief Serialize the engine struct to a save state and call the
 *        \ref unilayer_save to write that save state to storage.
 * \return Number of bytes written or error code.
 */
int32_t serial_save( const char* save_name, struct DSEKAI_STATE* state
   ) CODE_SECTION( "asnfns" ); /* Stuff it in ASN section even if it's a different fmt. */

/**
 * \brief Call the \ref unilayer_save to provide the save state with the
 *        given name and deserialize it into the given engine struct.
 * \return Number of bytes read or error code.
 */
int32_t serial_load( const char* save_name, struct DSEKAI_STATE* state
   ) CODE_SECTION( "asnfns" ); /* Stuff it in ASN section even if it's a different fmt. */

/*! \} */ /* dsekai_engines */

#endif /* !SERIAL_H */

