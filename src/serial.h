
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

/* int16_t engines_asn_load( RESOURCE_ID id, struct DSEKAI_STATE* state ); */

/**
 * \brief Serialize the engine struct to a save state.
 * \return Number of bytes written or error code.
 */
int32_t serial_save( const char* save_name, struct DSEKAI_STATE* state );

/*! \} */ /* dsekai_engines */

#endif /* !SERIAL_H */

