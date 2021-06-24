
#include "graphics.h"

#include "data/font8x8.h"

typedef void (*graphics_draw_px_cb)( uint16_t, uint16_t, GRAPHICS_COLOR );

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
            graphics_draw_px( x_orig + x, y_orig + y, pixel );
			}
			bitmask >>= 1;
		}
	}
}

void graphics_string_at(
   const char* s, uint16_t x_orig, uint16_t y_orig,
   GRAPHICS_COLOR color, uint8_t scale
) {
   uint16_t i = 0,
      x_o = 0; /* X offset. */

   while(
      '\0' != s[i] &&
      x_orig + FONT_W < SCREEN_REAL_W && /* On-screen (x-axis). */
      y_orig + FONT_H < SCREEN_REAL_H    /* On-screen (y-axis). */
   ) {
      graphics_char_at( s[i], x_orig + x_o, y_orig, color, scale );
      x_o += FONT_W + FONT_SPACE;
      i++;
   }
}

#ifdef USE_FAKE_CGA

void graphics_blit_at(
   const GRAPHICS_BITMAP* bmp, uint16_t x, uint16_t y, uint16_t w, uint16_t h
) {
   int y_offset = 0,
      byte_offset = 0,
      bit_offset = 0,
      x_scr_offset = 0,
      raw_byte = 0,
      masked_byte = 0;
   const uint8_t* bits = bmp->bits;

   for( y_offset = 0 ; h > y_offset ; y_offset++ ) {
      x_scr_offset = w - 1;

      /* Start at the last byte and move backwards. */
      for(
         byte_offset = ((bytes - 1) * 8) ;
         0 <= byte_offset ;
         byte_offset -= 8
      ) {
         /* Get full line and shift it to the current byte. */
         switch( w ) {
         case SPRITE_W:
            raw_byte = (((SPRITE_TYPE*)bits)[y_offset] >> byte_offset) & 0xff;
            break;
#if SPRITE_W != TILE_W
         case TILE_W:
            raw_byte = (((TILE_TYPE*)bits)[y_offset] >> byte_offset) & 0xff;
            break;
#endif
         case PATTERN_W:
            raw_byte = (((PATTERN_TYPE*)bits)[y_offset] >> byte_offset) & 0xff;
            break;
         }

         /* Iterate through the line 2 bits at a time. */
         for( bit_offset = 0 ; 8 > bit_offset ; bit_offset += 2 ) {
            masked_byte = raw_byte & 0x03;
            if( 0x01 == masked_byte ) {
               graphics_draw_px(
                  x + x_scr_offset, y + y_offset, GRAPHICS_COLOR_CYAN );
            } else if( 0x02 == masked_byte ) {
               graphics_draw_px(
                  x + x_scr_offset, y + y_offset, GRAPHICS_COLOR_MAGENTA );
            } else if( 0x03 == masked_byte ) {
               graphics_draw_px(
                  x + x_scr_offset, y + y_offset, GRAPHICS_COLOR_WHITE );
            } else if( 0x00 == masked_byte ) {
               graphics_draw_px(
                  x + x_scr_offset, y + y_offset, GRAPHICS_COLOR_BLACK );
            }

            raw_byte >>= 2;
            x_scr_offset--;
         }

      }
      assert( -1 == x_scr_offset );
   }
}

#endif /* USE_FAKE_CGA */

