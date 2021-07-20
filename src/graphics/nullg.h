
#ifndef NULLG_H
#define NULLG_H

#include "../graphics.h"

typedef int GRAPHICS_COLOR;
struct GRAPHICS_BITMAP {
   RESOURCE_ID id;
   uint8_t initialized;
   uint16_t ref_count;
};

struct GRAPHICS_ARGS {
};

#define GRAPHICS_COLOR_BLACK     0
#define GRAPHICS_COLOR_CYAN      1
#define GRAPHICS_COLOR_MAGENTA   2
#define GRAPHICS_COLOR_WHITE     3

#endif /* !NULLG_H */

