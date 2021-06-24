
#ifndef PALMG_H
#define PALMG_H

#include "../dstypes.h"

typedef IndexedColorType GRAPHICS_COLOR;

typedef struct GRAPHICS_BITMAP_SURFACE {
   MemHandle handle;
   BitmapPtr bitmap;
} GRAPHICS_BITMAP_SURFACE;

#include "../graphics.h"

#define GRAPHICS_COLOR_BLACK        0
#define GRAPHICS_COLOR_CYAN         1
#define GRAPHICS_COLOR_MAGENTA      2
#define GRAPHICS_COLOR_WHITE        3

#endif /* PALMG_H */

