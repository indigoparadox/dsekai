
#ifndef DOSG_H
#define DOSG_H

typedef uint8_t GRAPHICS_COLOR;

struct GRAPHICS_BITMAP {
   RESOURCE_ID id;
   uint8_t initialized;
   uint16_t ref_count;

   uint8_t* plane_1;
   uint8_t* plane_2;
   uint32_t w;
   uint32_t h;
   uint8_t palette;

   /* Unused Overrides */
   uint8_t res4;
};

struct GRAPHICS_ARGS {
   uint8_t mode;
};

#define GRAPHICS_COLOR_BLACK        0
#define GRAPHICS_COLOR_CYAN         1
#define GRAPHICS_COLOR_MAGENTA      2
#define GRAPHICS_COLOR_WHITE        3

#endif /* DOSG_H */

