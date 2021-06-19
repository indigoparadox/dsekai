
#include "palmg.h"

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

void graphics_draw_px( uint16_t x, uint16_t y, GRAPHICS_COLOR color ) {
}

void graphics_blit_at(
   const GRAPHICS_BITMAP* bmp,
   uint16_t x, uint16_t y, uint8_t w, uint8_t h, const int byte_width
) {
}

void graphics_blit_masked_at(
   const GRAPHICS_PATTERN* bmp, const GRAPHICS_MASK* mask,
   uint8_t mask_o_x, uint8_t mask_o_y,
   uint16_t x, uint16_t y, uint8_t w, uint8_t h, const int byte_width
) {
}

void graphics_draw_block(
   uint16_t x_orig, uint16_t y_orig, uint16_t w, uint16_t h,
   GRAPHICS_COLOR color
) {
}

