
#define SDLG_C
#include "../graphics.h"

#include <SDL.h>

#include "../data/drc.h"

SDL_Window* g_window = NULL;
SDL_Surface* g_screen = NULL;
SDL_Renderer* g_renderer = NULL;

volatile uint32_t g_ms;

const uint32_t gc_ms_target = 1000 / FPS;
static uint32_t g_ms_start = 0; 

void graphics_init() {
#ifdef DEBUG_CGA_EMU
   SDL_Rect area;
#endif /* DEBUG_CGA_EMU */

if( SDL_Init( SDL_INIT_EVERYTHING ) ) {
      printf( "error initializing SDL: %s\n", SDL_GetError() );
   }

   /*SDL_CreateWindowAndRenderer(
      SCREEN_REAL_W, SCREEN_REAL_H, 0, &g_window, &g_renderer );*/
   g_window = SDL_CreateWindow( "dsekai",
      SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
      SCREEN_REAL_W, SCREEN_REAL_H, 0 );
   assert( NULL != g_window );
   g_screen = SDL_GetWindowSurface( g_window );
   assert( NULL != g_screen );
   g_renderer = SDL_CreateSoftwareRenderer( g_screen );
   assert( NULL != g_renderer );

#ifdef DEBUG_CGA_EMU
   area.x = 0;
   area.y = 0;
   area.w = SCREEN_REAL_W;
   area.h = SCREEN_REAL_H;
   SDL_SetRenderDrawColor( g_renderer,  0, 0, 0, 255 );
   SDL_RenderFillRect( g_renderer, &area );
#endif /* DEBUG_CGA_EMU */
}

void graphics_shutdown() {
   SDL_DestroyWindow( g_window );
   SDL_Quit();
}

void graphics_flip() {
   /* SDL_RenderPresent( g_renderer ); */
   SDL_UpdateWindowSurface( g_window );
}

void graphics_loop_start() {
   g_ms_start = SDL_GetTicks();
}

void graphics_loop_end() {
   int16_t delta = 0;
   
   do {
      delta = gc_ms_target - (SDL_GetTicks() - g_ms_start);
   } while( 0 < delta );  
}

void graphics_draw_px( uint16_t x, uint16_t y, const GRAPHICS_COLOR color ) {
   int x_s = 0, y_s = 0;
   SDL_SetRenderDrawColor( g_renderer,  color->r, color->g, color->b, 255 );
   x *= SCREEN_SCALE;
   y *= SCREEN_SCALE;
   for( y_s = 0 ; SCREEN_SCALE > y_s ; y_s++ ) {
      for( x_s = 0 ; SCREEN_SCALE > x_s ; x_s++ ) {
         SDL_RenderDrawPoint( g_renderer, x + x_s, y + y_s );
      }
   }
}

void graphics_blit_at(
   const struct GRAPHICS_BITMAP* bmp,
   uint16_t x, uint16_t y, uint16_t w, uint16_t h
) {
   SDL_Rect dest_rect = {
      x * SCREEN_SCALE, 
      y * SCREEN_SCALE,
      w * SCREEN_SCALE, 
      h * SCREEN_SCALE};
   if( NULL == bmp || NULL == bmp->texture ) {
      return;
   }
   SDL_RenderCopy( g_renderer, bmp->texture, NULL, &dest_rect );
}

#if 0
void graphics_blit_masked_at(
   const GRAPHICS_PATTERN* bmp, const GRAPHICS_MASK* mask,
   uint8_t mask_o_x, uint8_t mask_o_y,
   uint16_t x, uint16_t y, uint8_t w, uint8_t h, const int byte_width
) {
   GRAPHICS_PATTERN pattern_tmp;
   int x_offset = 0, y_offset = 0;

   memcpy( &pattern_tmp, bmp, sizeof( GRAPHICS_PATTERN ) );

   /* Apply the transparency mask to the pattern. */
   for( y_offset = 0 ; PATTERN_H > y_offset ; y_offset++ ) {
      pattern_tmp.bits[y_offset] = bmp->bits[y_offset];
      for( x_offset = 0 ; 8 > x_offset ; x_offset++ ) {
         /* Perform endian "conversion" (though masks are one byte). */
         if( x_offset >= 4 ) {
            /* Compare each row, bit by (double) bit. */
            if( !(mask->bits[y_offset] & (0x01 << x_offset)) ) {
               pattern_tmp.bits[y_offset] &= ~(0x3 << (x_offset));
            }
         } else {
            /* Compare each row, bit by (double) bit. */
            if( !(mask->bits[y_offset] & (0x01 << x_offset)) ) {
               pattern_tmp.bits[y_offset] &= ~(0x3 << (x_offset + 8));
            }
         }
      }
   }

   graphics_pattern_at( &pattern_tmp, x, y );
}
#endif

void graphics_draw_block(
   uint16_t x_orig, uint16_t y_orig, uint16_t w, uint16_t h,
   const GRAPHICS_COLOR color
) {
   SDL_Rect area;

#ifdef SCALE_2X
   area.x = x_orig * 2;
   area.y = y_orig * 2;
   area.w = w * 2;
   area.h = h * 2;
#else
   area.x = x_orig;
   area.y = y_orig;
   area.w = w;
   area.h = h;
#endif /* SCALE_2X */

   SDL_SetRenderDrawColor( g_renderer,  color->r, color->g, color->b, 255 );
   SDL_RenderFillRect( g_renderer, &area );
}

/*
 * @return 1 if bitmap is loaded and 0 otherwise.
 */
int32_t graphics_load_bitmap( uint32_t id, struct GRAPHICS_BITMAP** b ) {
   uint8_t* buffer = NULL;
   int32_t buffer_sz = 0;
   SDL_RWops* bmp_stream;

   assert( NULL != b );
   assert( NULL == *b );

   *b = calloc( 1, sizeof( struct GRAPHICS_BITMAP ) );
   assert( 0 == (*b)->ref_count );
   (*b)->ref_count++;

   buffer_sz = drc_get_resource(
      DRC_ARCHIVE, *(uint32_t*)DRC_BMP_TYPE, id, &buffer );
   if( 0 >= buffer_sz ) {
      assert( NULL == buffer );
      return buffer_sz;
   }

   bmp_stream = SDL_RWFromMem( buffer, buffer_sz );
   (*b)->surface = SDL_LoadBMP_RW( bmp_stream, 1 ); /* Free stream on close. */
   if( NULL == (*b)->surface ) {
      fprintf( stderr, "unable to load bitmap %u: %s\n",
         id, SDL_GetError() );
      buffer_sz = -1;
      goto cleanup;
   }
   free( buffer ); /* Free resource memory. */
   (*b)->texture = SDL_CreateTextureFromSurface( g_renderer, (*b)->surface );
   assert( NULL != (*b)->texture );

cleanup:
   return buffer_sz;
}

/*
 * @return 1 if bitmap is unloaded and 0 otherwise.
 */
int32_t graphics_unload_bitmap( struct GRAPHICS_BITMAP** b ) {
   assert( NULL != *b );
   (*b)->ref_count--;
   if( 0 >= (*b)->ref_count ) {
      SDL_DestroyTexture( (*b)->texture );
      SDL_FreeSurface( (*b)->surface );
      free( *b );
      *b = NULL;
      return 1;
   }
   return 0;
}

