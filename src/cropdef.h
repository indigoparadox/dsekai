
#ifndef CROPDEF_H
#define CROPDEF_H

struct CROP_DEF {
   RESOURCE_ID sprite;
   char name[CROP_NAME_MAX];
   uint8_t gid;
   uint8_t flags;
   uint32_t cycle;
};

#endif /* !CROPDEF_H */

