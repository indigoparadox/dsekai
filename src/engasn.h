
#ifndef ENGASN_H
#define ENGASN_H

#define ENGINES_ASN_SAVE_ERROR -1

/* int16_t engines_asn_load( RESOURCE_ID id, struct DSEKAI_STATE* state ); */
int32_t engines_asn_save( const char* save_name, struct DSEKAI_STATE* state );

#endif /* !ENGASN_H */

