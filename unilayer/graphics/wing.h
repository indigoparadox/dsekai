
#ifndef WING_H
#define WING_H

#include "../dstypes.h"

typedef COLORREF GRAPHICS_COLOR;
struct GRAPHICS_BITMAP {
   RESOURCE_ID id;
   uint8_t initialized;
   uint16_t ref_count;
   HBITMAP bitmap;
   HDC hdc;

   /* Unused Overrides */
   uint32_t res1;
   uint32_t res2;
   uint8_t res3;
   uint8_t res4;
};

struct GRAPHICS_ARGS {
   int cmd_show;
   uint8_t scale;
};

#define WIN_GFX_TIMER_ID 6001

#define GRAPHICS_COLOR_BLACK        RGB(0, 0, 0)
#define GRAPHICS_COLOR_CYAN         RGB(0, 255, 255)
#define GRAPHICS_COLOR_MAGENTA      RGB(255, 0, 255)
#define GRAPHICS_COLOR_WHITE        RGB(255, 255, 255)

#endif /* WING_H */

