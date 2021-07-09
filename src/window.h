
#ifndef WINDOW_H
#define WINDOW_H

#define WINDOW_COUNT_MAX 10

#define WINDOW_STATUS_EMPTY    0
#define WINDOW_STATUS_OPENING  1
#define WINDOW_STATUS_VISIBLE  2
#define WINDOW_STATUS_MODAL  3

struct WINDOW_FRAME {
   uint32_t tr;
   uint32_t tl;
   uint32_t br;
   uint32_t bl;
   uint32_t t;
   uint32_t b;
   uint32_t r;
   uint32_t l;
   uint32_t c;
};

struct WINDOW {
   uint32_t id;
   uint8_t status;
   uint8_t dirty;
   uint8_t frame_idx;
   uint8_t controls_count;
   MEMORY_HANDLE controls_handle;
   uint16_t x;
   uint16_t y;
   uint16_t w;
   uint16_t h;
};

void window_init();
void window_shutdown();
int window_draw_all( struct DSEKAI_STATE* );
int16_t window_push(
   uint32_t, uint8_t,
   int16_t, int16_t, int16_t, int16_t, uint8_t, struct DSEKAI_STATE* );
void window_pop( uint32_t, struct DSEKAI_STATE* );
int16_t window_modal( struct DSEKAI_STATE* );

#endif /* WINDOW_H */

