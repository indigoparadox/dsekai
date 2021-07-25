
#ifndef PALMG_H
#define PALMG_H

typedef IndexedColorType GRAPHICS_COLOR;
struct GRAPHICS_BITMAP {
   RESOURCE_ID id;
   uint8_t initialized;
   uint16_t ref_count;

   MemHandle handle;
   BitmapPtr bitmap;

   /* Unused Overrides */
   uint32_t res1;
   uint32_t res2;
   uint8_t res3;
   uint8_t res4;
};

struct GRAPHICS_ARGS {
};

#define GRAPHICS_COLOR_BLACK        0
#define GRAPHICS_COLOR_CYAN         1
#define GRAPHICS_COLOR_MAGENTA      2
#define GRAPHICS_COLOR_WHITE        3

#endif /* PALMG_H */

