
#ifndef JSON_H
#define JSON_H

#include "../../src/dstypes.h"

#define JSON_TYPE_LIST_
#define JSON_TYPE_OBJECT         

struct JSON_BASE {
   uint8_t json_type;
   struct JSON_BASE* parent;
};

struct JSON_ATTRIB {
   uint8_t json_type;

   struct JSON_ATTRIB* prev_sibling;
   struct JSON_ATTRIB* next_sibling;
};

struct JSON_OBJECT {
   uint8_t json_type;

   struct JSON_BASE* first_child;
   struct JSON_ATTRIB* prev_sibling;
   struct JSON_ATTRIB* next_sibling;
};

struct JSON_LIST {
   uint8_t json_type;

   struct JSON_LIST_ITEM* first_child;
   struct JSON_ATTRIB* prev_sibling;
   struct JSON_ATTRIB* next_sibling;
};

struct JSON_OBJECT* json_parse_buffer(
   const uint8_t* buffer, uint32_t buffer_sz );

#endif /* JSON_H */

