
#ifndef PALMG_H
#define PALMG_H

#include <PalmOS.h>

#include "../graphics.h"

typedef IndexedColorType GRAPHICS_COLOR;
struct GRAPHICS_BITMAP {
   uint32_t id;
   uint8_t initialized;
   uint16_t ref_count;
   MemHandle handle;
   BitmapPtr bitmap;
};

#define GRAPHICS_COLOR_BLACK        0
#define GRAPHICS_COLOR_CYAN         1
#define GRAPHICS_COLOR_MAGENTA      2
#define GRAPHICS_COLOR_WHITE        3

#endif /* PALMG_H */

