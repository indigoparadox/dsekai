
#ifndef WINDOW_H
#define WINDOW_H

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
   uint8_t dirty;
   uint16_t x;
   uint16_t y;
   uint16_t w;
   uint16_t h;
   uint8_t frame_idx;
   char* strings[WINDOW_STRINGS_MAX];
   GRAPHICS_COLOR strings_color;
   uint8_t strings_count;
};

void window_init();
void window_shutdown();
int window_draw_all( struct DSEKAI_STATE* );
void window_push(
   uint16_t, uint16_t, uint16_t, uint16_t, uint8_t, struct DSEKAI_STATE* );
void window_pop( struct DSEKAI_STATE* );

#endif /* WINDOW_H */

