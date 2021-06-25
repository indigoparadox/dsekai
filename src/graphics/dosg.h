
#ifndef DOSG_H
#define DOSG_H

#include "../dstypes.h"
#include "../graphics.h"

typedef uint8_t GRAPHICS_COLOR;

struct GRAPHICS_BITMAP {
   uint32_t id;
   uint8_t initialized;
   uint16_t ref_count;
   uint32_t w;
   uint32_t h;
   uint8_t palette;
   uint8_t* plane_1;
   uint8_t* plane_2;
};

typedef struct GRAPHICS_PATTERN {
   PATTERN_TYPE bits[PATTERN_H]; } GRAPHICS_PATTERN;

#define GRAPHICS_COLOR_BLACK        0
#define GRAPHICS_COLOR_CYAN         1
#define GRAPHICS_COLOR_MAGENTA      2
#define GRAPHICS_COLOR_WHITE        3

#endif /* DOSG_H */

