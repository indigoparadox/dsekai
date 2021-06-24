
#ifndef DOSG_H
#define DOSG_H

typedef uint8_t GRAPHICS_COLOR;

typedef struct GRAPHICS_BITMAP_SURFACE {
   uint32_t w;
   uint32_t h;
   uint8_t* plane_1;
   uint8_t* plane_2;
} GRAPHICS_BITMAP_SURFACE;

#include "../dstypes.h"
#include "../graphics.h"

#define GRAPHICS_COLOR_BLACK        0
#define GRAPHICS_COLOR_CYAN         1
#define GRAPHICS_COLOR_MAGENTA      2
#define GRAPHICS_COLOR_WHITE        3

#endif /* DOSG_H */

