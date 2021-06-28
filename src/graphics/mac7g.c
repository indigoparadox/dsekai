
#include "../graphics.h"

void graphics_init() {
}

void graphics_shutdown() {
}

void graphics_flip() {
}

void graphics_loop_start() {
}

void graphics_loop_end() {
}

void graphics_draw_px( uint16_t x, uint16_t y, const GRAPHICS_COLOR color ) {
}

void graphics_platform_blit_at(
   const struct GRAPHICS_BITMAP* bmp,
   uint16_t x, uint16_t y, uint16_t w, uint16_t h
) {
}

void graphics_draw_block(
   uint16_t x_orig, uint16_t y_orig, uint16_t w, uint16_t h,
   const GRAPHICS_COLOR color
) {
}

int32_t graphics_load_bitmap( uint32_t id, struct GRAPHICS_BITMAP* b ) {
}

int32_t graphics_unload_bitmap( struct GRAPHICS_BITMAP* b ) {
}

