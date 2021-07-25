
#define GRAPHICS_C
#include "../unilayer.h"

int16_t graphics_platform_init( struct GRAPHICS_ARGS* args ) {
   return 1;
}

void graphics_platform_shutdown( struct GRAPHICS_ARGS* args ) {

}

void graphics_flip( struct GRAPHICS_ARGS* args ) {
}

void graphics_loop_start() {
}

void graphics_loop_end() {
}

void graphics_draw_px( uint16_t x, uint16_t y, const GRAPHICS_COLOR color ) {
}

void graphics_draw_block(
   uint16_t x_orig, uint16_t y_orig, uint16_t w, uint16_t h,
   const GRAPHICS_COLOR color
) {
}

void graphics_draw_line(
   uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t thickness,
   const GRAPHICS_COLOR color
) {
}

void graphics_draw_rect(
   uint16_t x_orig, uint16_t y_orig, uint16_t w, uint16_t h,
   uint16_t thickness, const GRAPHICS_COLOR color
) {
}

int graphics_platform_blit_at(
   const struct GRAPHICS_BITMAP* bmp,
   uint16_t x, uint16_t y, uint16_t w, uint16_t h
) {
}

int16_t graphics_platform_load_bitmap(
   RESOURCE_BITMAP_HANDLE res_handle, struct GRAPHICS_BITMAP* b
) {
   return 0;
}

int16_t graphics_unload_bitmap( struct GRAPHICS_BITMAP* b ) {
   return 0;
}

