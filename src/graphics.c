
#include "graphics.h"

#include "data/font8x8.h"

int graphics_platform_blit_at(
   const struct GRAPHICS_BITMAP*, uint16_t, uint16_t, uint16_t, uint16_t );

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

/*
 * @return 1 if blit was successful and 0 otherwise.
 */
int graphics_blit_at(
   struct GRAPHICS_BITMAP* bmp,
   uint16_t x, uint16_t y, uint16_t w, uint16_t h
) {
   int retval = 0;

   if( NULL == bmp ) {
      /* Can't do anything. */
      error_printf( "tried to blit empty bitmap" );
      goto cleanup;
   }

   if( !bmp->initialized && 0 < bmp->id ) {
      /* Try to load uninitialized, pre-populated bitmap resource. */
      retval = graphics_load_bitmap( 0, bmp );
      if( !retval ) {
         error_printf( "failed to lazy load bitmap" );
         goto cleanup;
      }
   }

   if( bmp->initialized ) {
      retval = graphics_platform_blit_at( bmp, x, y, w, h );
      if( !retval ) {
         error_printf( "failed to blit bitmap" );
         goto cleanup;
      }
   }

cleanup:
   return retval;
}

