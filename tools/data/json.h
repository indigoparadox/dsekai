
#ifndef JSON_H
#define JSON_H

#include "../../src/dstypes.h"

#define JSON_TYPE_LIST_
#define JSON_TYPE_OBJECT         

#ifndef JSON_C
#define JSMN_HEADER
#endif /* JSON_C */
#define JSMN_PARENT_LINKS
#include "jsmn.h"

int16_t json_token_id_from_path(
   const char*, jsmntok_t*, uint16_t, const uint8_t* );

#endif /* JSON_H */
