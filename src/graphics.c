
#include "graphics.h"

#include "data/font8x8.h"

void graphics_sprite_at(
   const uint8_t spr[SPRITE_H], uint16_t x_orig, uint16_t y_orig, uint8_t color
) {
	int x = 0;
	int y = 0;
	int bitmask = 0;
	int pixel = 0;

	for( y = 0 ; SPRITE_H > y ; y++ ) {
		bitmask = spr[y];
		for( x = 0 ; SPRITE_W > x ; x++ ) {
			if( bitmask & 0x01 ) {
				pixel = color;
			} else {
				pixel = 0;
			}
			graphics_draw_px( x_orig + x, y_orig + y, pixel );
			bitmask >>= 1;
		}
	}
}

void graphics_char_at(
   const char c, uint16_t x_orig, uint16_t y_orig, uint8_t color
) {
	int x = 0;
	int y = 0;
	int bitmask = 0;
	int pixel = 0;

	for( y = 0 ; FONT_H > y ; y++ ) {
		bitmask = gc_font8x8_basic[c][y];
		for( x = 0 ; FONT_W > x ; x++ ) {
			if( bitmask & 0x01 ) {
				pixel = color;
			} else {
				pixel = 0;
			}
			graphics_draw_px( x_orig + x, y_orig + y, pixel );
			bitmask >>= 1;
		}
	}
}

void graphics_string_at(
   const char* s, uint16_t slen, uint16_t x_orig, uint16_t y_orig, uint8_t color
) {
   uint16_t i = 0,
      x_o = 0; /* X offset. */

   while(
      i < slen &&
      '\0' != s[i] &&
      x_orig + FONT_W < SCREEN_W && /* On-screen (x-axis). */
      y_orig + FONT_H < SCREEN_H    /* On-screen (y-axis). */
   ) {
      graphics_char_at( s[i], x_orig + x_o, y_orig, color );
      x_o += FONT_W + FONT_SPACE;
      i++;
   }
}

