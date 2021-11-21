
#ifndef ASN_H
#define ASN_H

/**
 * \addtogroup dsekai_asn ASN.1 Format
 * \brief General binary format for storing and retrieving data.
 *
 * \{
 */

/*! \file asn.h
 *  \brief Functions and macros for dealing with ASN.1 formatted data.
 */

/**
 * \addtogroup dsekai_asn_types ASN.1 Data Types
 *
 * \{
 */

#define ASN_STRING     0x16
#define ASN_INT        0x02
#define ASN_BLOB       0x04
#define ASN_SEQUENCE   0x30

/*! \} */

#define ASN_ERROR_INVALID_VALUE_SZ -1
#define ASN_ERROR_UNABLE_TO_ALLOCATE -2

int32_t asn_write_int(
   uint8_t** p_buffer, int32_t* p_buffer_sz, int32_t idx, int32_t value );
int32_t asn_write_string(
   uint8_t** p_buffer, int32_t* p_buffer_sz, int32_t idx,
   uint8_t* source, int32_t source_sz );

/*! \} */

#endif /* !ASN_H */

