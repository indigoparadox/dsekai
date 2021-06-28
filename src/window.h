
#ifndef WINDOW_H
#define WINDOW_H

#include "graphics.h"

#define WINDOW_COUNT_MAX 10

#define WINDOW_STATE_EMPTY    0
#define WINDOW_STATE_OPENING  1
#define WINDOW_STATE_VISIBLE  2

struct WINDOW_FRAME {
   struct GRAPHICS_BITMAP tr;
   struct GRAPHICS_BITMAP tl;
   struct GRAPHICS_BITMAP br;
   struct GRAPHICS_BITMAP bl;
   struct GRAPHICS_BITMAP t;
   struct GRAPHICS_BITMAP b;
   struct GRAPHICS_BITMAP r;
   struct GRAPHICS_BITMAP l;
   struct GRAPHICS_BITMAP c;
};

struct WINDOW {
   uint8_t state;
   uint8_t dirty;
   uint16_t x;
   uint16_t y;
   uint16_t w;
   uint16_t h;
   struct WINDOW_FRAME* frame;
   char* strings[WINDOW_STRINGS_MAX];
   GRAPHICS_COLOR strings_color;
   uint8_t strings_count;
};

void window_init();
void window_draw_all();
uint8_t windows_visible();
struct WINDOW* window_push();
void window_pop();

#endif /* WINDOW_H */

