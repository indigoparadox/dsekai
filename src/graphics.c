
#include "graphics.h"

#include "data/font8x8.h"

typedef void (*graphics_draw_px_cb)( uint16_t, uint16_t, GRAPHICS_COLOR );

void graphics_draw_px_2x( uint16_t x, uint16_t y, const GRAPHICS_COLOR color ) {
   graphics_draw_px( x * 2, y * 2, color );
   graphics_draw_px( x * 2, y * 2 + 1, color );
   graphics_draw_px( x * 2 + 1, y * 2, color );
   graphics_draw_px( x * 2 + 1, y * 2 + 1, color );
}

void graphics_char_at(
   const char c, uint16_t x_orig, uint16_t y_orig, GRAPHICS_COLOR color,
   uint8_t scale
) {
	int x = 0;
	int y = 0;
	int bitmask = 0;
	GRAPHICS_COLOR pixel = GRAPHICS_COLOR_BLACK;

	for( y = 0 ; FONT_H > y ; y++ ) {
		bitmask = gc_font8x8_basic[c][y];
		for( x = 0 ; FONT_W > x ; x++ ) {
			if( bitmask & 0x01 ) {
				pixel = color;
			} else {
				pixel = GRAPHICS_COLOR_BLACK;
			}
			graphics_draw_px( x_orig + x, y_orig + y, pixel );
			bitmask >>= 1;
		}
	}
}

void graphics_string_at(
   const char* s, uint16_t slen, uint16_t x_orig, uint16_t y_orig,
   GRAPHICS_COLOR color, uint8_t scale
) {
   uint16_t i = 0,
      x_o = 0; /* X offset. */

   while(
      i < slen &&
      '\0' != s[i] &&
      x_orig + FONT_W < SCREEN_W && /* On-screen (x-axis). */
      y_orig + FONT_H < SCREEN_H    /* On-screen (y-axis). */
   ) {
      graphics_char_at( s[i], x_orig + x_o, y_orig, color, scale );
      x_o += FONT_W + FONT_SPACE;
      i++;
   }
}

