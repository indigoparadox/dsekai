
#ifndef JSON_H
#define JSON_H

#include "../../src/dstypes.h"

struct JSON_BASE {
   uint8_t json_type;
};

struct JSON_LIST_ITEM {
   uint8_t json_type;

   struct JSON_BASE* child;
   struct JSON_LIST_ITEM* next;
};

struct JSON_KV_ITEM {
   uint8_t json_type;

   struct JSON_BASE* child;
   struct JSON_KV_ITEM* next;
};

struct JSON_OBJECT {
   uint8_t json_type;

   struct JSON_KV_ITEM* first_child;
};

struct JSON_LIST {
   uint8_t json_type;

   struct JSON_LIST_ITEM* first_child;
};

struct JSON_OBJECT* json_parse_buffer(
   const uint8_t* buffer, uint32_t buffer_sz );

#endif /* JSON_H */

