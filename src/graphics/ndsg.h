
#ifndef NDSG_H
#define NDSG_H

#include "../dstypes.h"

#include <nds.h>

typedef uint16_t GRAPHICS_COLOR;
struct GRAPHICS_BITMAP {
   RESOURCE_ID id;
   uint8_t initialized;
   uint16_t ref_count;

   /* Unused Overrides */
   void* resa;
   void* resb;
   uint32_t res1;
   uint32_t res2;
   uint8_t res3;
   uint8_t res4;
};

struct GRAPHICS_ARGS {
};

#define GRAPHICS_COLOR_BLACK        RGB15(0, 0, 0)
#define GRAPHICS_COLOR_CYAN         RGB15(0, 255, 255)
#define GRAPHICS_COLOR_MAGENTA      RGB15(255, 0, 255)
#define GRAPHICS_COLOR_WHITE        RGB15(255, 255, 255)

#endif /* NDSG_H */

