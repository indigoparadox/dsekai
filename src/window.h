
#ifndef WINDOW_H
#define WINDOW_H

#include "graphics.h"

#define WINDOW_COUNT_MAX 10

#define WINDOW_STATE_EMPTY    0
#define WINDOW_STATE_OPENING  1
#define WINDOW_STATE_VISIBLE  2

struct WINDOW {
   uint8_t state;
   uint8_t dirty;
   uint16_t x;
   uint16_t y;
   uint16_t w;
   uint16_t h;
   const struct GRAPHICS_PATTERN* pattern;
   const char* strings[WINDOW_STRINGS_MAX];
   const GRAPHICS_COLOR* strings_color;
   uint8_t strings_count;
};

void window_init();
void window_draw_all();
uint8_t windows_visible();
struct WINDOW* window_push();
void window_pop();

#endif /* WINDOW_H */

