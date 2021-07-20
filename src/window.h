
#ifndef WINDOW_H
#define WINDOW_H

#define WINDOW_COUNT_MAX 10

#define WINDOW_STATUS_EMPTY    0
#define WINDOW_STATUS_OPENING  1
#define WINDOW_STATUS_VISIBLE  2
#define WINDOW_STATUS_MODAL  3

struct WINDOW_FRAME {
   RESOURCE_ID tr;
   RESOURCE_ID tl;
   RESOURCE_ID br;
   RESOURCE_ID bl;
   RESOURCE_ID t;
   RESOURCE_ID b;
   RESOURCE_ID r;
   RESOURCE_ID l;
   RESOURCE_ID c;
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

