
#define GRAPHICS_C
#include "unilayer.h"

#ifdef USE_SOFTWARE_TEXT
#include "data/font8x8.h"
#endif /* USE_SOFTWARE_TEXT */

static MEMORY_HANDLE gs_graphics_cache_handle = NULL;
static int16_t gs_graphics_cache_sz = 0;

int16_t graphics_init( struct GRAPHICS_ARGS* args ) {
   int16_t retval = 1;

   retval = graphics_platform_init( args );
   if( !retval ) {
      goto cleanup;
   }

   gs_graphics_cache_handle = memory_alloc(
      GRAPHICS_CACHE_INITIAL_SZ, sizeof( struct GRAPHICS_BITMAP ) );
   gs_graphics_cache_sz = GRAPHICS_CACHE_INITIAL_SZ;

cleanup:
   return retval;
}

void graphics_shutdown( struct GRAPHICS_ARGS* args ) {
   int16_t i = 0;
   struct GRAPHICS_BITMAP* bitmaps = NULL;

   bitmaps = (struct GRAPHICS_BITMAP*)memory_lock( gs_graphics_cache_handle );
   for( i = 0 ; gs_graphics_cache_sz > i ; i++ ) {
      if( 1 == bitmaps[i].initialized ) {
         graphics_unload_bitmap( &(bitmaps[i]) );
      }
   }
   bitmaps = (struct GRAPHICS_BITMAP*)memory_unlock( gs_graphics_cache_handle );

   memory_free( gs_graphics_cache_handle );

   graphics_platform_shutdown( args );
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
   const char* str, uint16_t str_sz, uint16_t x_orig, uint16_t y_orig,
   GRAPHICS_COLOR color, uint8_t scale
) {
   uint16_t i = 0,
      x_o = 0; /* X offset. */

   while(
      '\0' != str[i]
#if defined( SCREEN_REAL_W ) && defined( SCREEN_REAL_H )
      &&
      x_orig + FONT_W < SCREEN_REAL_W && /* On-screen (x-axis). */
      y_orig + FONT_H < SCREEN_REAL_H    /* On-screen (y-axis). */
#endif /* SCREEN_REAL_W && SCREEN_REAL_H */
   ) {
      graphics_char_at( str[i], x_orig + x_o, y_orig, color, scale );
      x_o += FONT_W + FONT_SPACE;
      i++;
   }
}

void graphics_string_sz(
   const char* str, uint16_t str_sz, uint8_t scale, struct GRAPHICS_RECT* sz_out
) {
   sz_out->w = memory_strnlen_ptr( str, str_sz ) * (FONT_W + FONT_SPACE);
   if( sz_out->w > 0 ) {
      sz_out -= FONT_SPACE; /* Remove trailing space. */
   }
   sz_out->h = FONT_H;
}

#endif /* USE_SOFTWARE_TEXT */

/** 
 * \brief Load the graphics resource into a handle and pass that into the
 *        platform-specific loader.
 */
int16_t graphics_load_bitmap( RESOURCE_ID id, struct GRAPHICS_BITMAP* b ) {
   int16_t retval = 0;
   RESOURCE_BITMAP_HANDLE bitmap_handle = NULL;
   
   /* Load resource into buffer. */
   bitmap_handle = resource_get_bitmap_handle( id );
   if( NULL == bitmap_handle ) {
      retval = 0;
      error_printf( "unable to get resource handle" );
      goto cleanup;
   }

   assert( NULL != b );

   resource_assign_id( b->id, id );
   retval = graphics_platform_load_bitmap( bitmap_handle, b );
   if( !retval ) {
      error_printf( "failed to lazy load bitmap" );
      goto cleanup;
   }

   /* Assume the load was a success. */
   b->ref_count++;
   b->initialized = 1;

cleanup:

   /* The platform-specific loader should dispose of the resource if needed.
    * This is because some platforms use the resource directly.
    */

   return retval;
}

/**
 * @return 1 if blit was successful and 0 otherwise.
 */
int16_t graphics_blit_at(
   RESOURCE_ID res_id, uint16_t x, uint16_t y, uint16_t w, uint16_t h
) {
   int16_t retval = 0,
      i = 0;
   struct GRAPHICS_BITMAP* bitmaps = NULL,
      * bitmap_blit = NULL;

   bitmaps = (struct GRAPHICS_BITMAP*)memory_lock( gs_graphics_cache_handle );

   /* Try to find the bitmap already in the cache. */
   for( i = 0 ; gs_graphics_cache_sz > i ; i++ ) {
      if( resource_compare_id( bitmaps[i].id, res_id ) ) {
         bitmap_blit = &(bitmaps[i]);
         break;
      }
   }

   if( NULL == bitmap_blit ) {
      /* Bitmap not found. */
      debug_printf( 1, "bitmap not found in cache; loading..." );
      for( i = 0 ; gs_graphics_cache_sz > i ; i++ ) {
         if( 0 == bitmaps[i].initialized ) {
            if( graphics_load_bitmap( res_id, &(bitmaps[i]) ) ) {
               bitmap_blit = &(bitmaps[i]);
            }
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
      /* error_printf( "failed to blit bitmap" ); */
      goto cleanup;
   }

cleanup:

   if( NULL != bitmaps ) {
      bitmaps = (struct GRAPHICS_BITMAP*)memory_unlock(
         gs_graphics_cache_handle );
   }

   return retval;
}

