
#ifndef NULLG_H
#define NULLG_H

#include "../graphics.h"

typedef int GRAPHICS_COLOR;
struct GRAPHICS_BITMAP {
   uint32_t id;
   uint8_t initialized;
   uint16_t ref_count;
};

typedef struct GRAPHICS_PATTERN {
   PATTERN_TYPE bits[PATTERN_H]; } GRAPHICS_PATTERN;

#define GRAPHICS_COLOR_BLACK     0
#define GRAPHICS_COLOR_CYAN      1
#define GRAPHICS_COLOR_MAGENTA   2
#define GRAPHICS_COLOR_WHITE     3

#endif /* !NULLG_H */

