
#ifndef TMASN_H
#define TMASN_H

/**
 * \addtogroup dsekai_tilemaps
 *
 * \{
 */

/**
 * \addtogroup asn_tilemaps ASN.1 Tilemaps
 * \brief Extremely specific dialect of ASN.1 used to store tilemaps in a
 *        compact and simple binary format.
 *
 * \{
 */

/*! \file tmasn.h
 *  \brief Definitions and functions for reading an ASN.1-formatted ::TILEMAP.
 */

/*! \brief Read size returned when there was a parsing error. */
#define TILEMAP_ASN_ERROR_READ -1

int16_t tilemap_asn_load( RESOURCE_ID id, struct TILEMAP* t );

/*! \} */

/*! \} */

#endif /* !TMASN_H */

