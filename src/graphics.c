
#include "dstypes.h"

#include "data/font8x8.h"

static MEMORY_HANDLE gs_graphics_cache_handle = NULL;
static int16_t gs_graphics_cache_sz = 0;

int graphics_platform_blit_at(
   const struct GRAPHICS_BITMAP*, uint16_t, uint16_t, uint16_t, uint16_t );
int graphics_platform_init();
int graphics_platform_shutdown();

int16_t graphics_init() {
   int16_t retval = 1;
   struct GRAPHICS_BITMAP* bitmaps = NULL;

   retval = graphics_platform_init();
   if( !retval ) {
      goto cleanup;
   }

   gs_graphics_cache_handle = memory_alloc(
      GRAPHICS_CACHE_INITIAL_SZ, sizeof( struct GRAPHICS_BITMAP ) );
   gs_graphics_cache_sz = GRAPHICS_CACHE_INITIAL_SZ;

   /* bitmaps = memory_lock( gs_graphics_cache_handle );
   memory_zero_ptr( bitmaps, GRAPHICS_CACHE_INITIAL_SZ * sizeof( struct GRAPHICS_BITMAP ) );
   bitmaps = memory_unlock( gs_graphics_cache_handle ); */

cleanup:
   return retval;
}

void graphics_shutdown() {
   int16_t i = 0;
   struct GRAPHICS_BITMAP* bitmaps = NULL;

   bitmaps = memory_lock( gs_graphics_cache_handle );
   for( i = 0 ; gs_graphics_cache_sz > i ; i++ ) {
      if( 1 == bitmaps[i].initialized ) {
         graphics_unload_bitmap( &(bitmaps[i]) );
      }
   }
   bitmaps = memory_unlock( gs_graphics_cache_handle );

   memory_free( gs_graphics_cache_handle );

   graphics_platform_shutdown();
}

#ifdef USE_SOFTWARE_TEXT

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
   const char* s, uint16_t s_len, uint16_t x_orig, uint16_t y_orig,
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

#endif /* USE_SOFTWARE_TEXT */

/*
 * @return 1 if blit was successful and 0 otherwise.
 */
int16_t graphics_blit_at(
   uint32_t resource, uint16_t x, uint16_t y, uint16_t w, uint16_t h
) {
   int16_t retval = 0,
      i = 0;
   struct GRAPHICS_BITMAP* bitmaps = NULL,
      * bitmap_blit = NULL;

   bitmaps = memory_lock( gs_graphics_cache_handle );

   /* Try to find the bitmap already in the cache. */
   for( i = 0 ; gs_graphics_cache_sz > i ; i++ ) {
      if( bitmaps[i].id == resource ) {
         bitmap_blit = &(bitmaps[i]);
         break;
      }
   }

   if( NULL == bitmap_blit ) {
      /* Bitmap not found. */
      debug_printf( 1, "bitmap %u not found in cache; loading...", resource );
      for( i = 0 ; gs_graphics_cache_sz > i ; i++ ) {
         if( 0 == bitmaps[i].initialized ) {
            bitmaps[i].id = resource;
            retval = graphics_load_bitmap( resource, &(bitmaps[i]) );
            if( !retval ) {
               error_printf( "failed to lazy load bitmap" );
               goto cleanup;
            }
            bitmap_blit = &(bitmaps[i]);
            break;
         }
      }
   }

   if( NULL == bitmap_blit ) {
      error_printf( "unable to load bitmap; cache full" );
      goto cleanup;
   }

   retval = graphics_platform_blit_at( bitmap_blit, x, y, w, h );
   if( !retval ) {
      error_printf( "failed to blit bitmap" );
      goto cleanup;
   }

cleanup:

   if( NULL != bitmaps ) {
      bitmaps = memory_unlock( gs_graphics_cache_handle );
   }

   return retval;
}

