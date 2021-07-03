
#define SDLG_C
#include "../graphics.h"

#include <SDL.h>

#include "../data/drc.h"
#include "../memory.h"

SDL_Window* g_window = NULL;
SDL_Surface* g_screen = NULL;
SDL_Renderer* g_renderer = NULL;

volatile uint32_t g_ms;

const uint32_t gc_ms_target = 1000 / FPS;
static uint32_t g_ms_start = 0; 

/*
 * @return 1 if init was successful and 0 otherwise.
 */
int graphics_init() {
#ifdef DEBUG_CGA_EMU
   SDL_Rect area;
#endif /* DEBUG_CGA_EMU */

   if( SDL_Init( SDL_INIT_EVERYTHING ) ) {
      error_printf( "error initializing SDL: %s", SDL_GetError() );
   }

   /*SDL_CreateWindowAndRenderer(
      SCREEN_REAL_W, SCREEN_REAL_H, 0, &g_window, &g_renderer );*/
   g_window = SDL_CreateWindow( "dsekai",
      SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
      SCREEN_REAL_W, SCREEN_REAL_H, 0 );
   if( NULL ) {
      return 0;
   }
   g_screen = SDL_GetWindowSurface( g_window );
   if( NULL == g_screen ) {
      return 0;
   }
   g_renderer = SDL_CreateSoftwareRenderer( g_screen );
   if( NULL == g_renderer ) {
      return 0;
   }

#ifdef DEBUG_CGA_EMU
   area.x = 0;
   area.y = 0;
   area.w = SCREEN_REAL_W;
   area.h = SCREEN_REAL_H;
   SDL_SetRenderDrawColor( g_renderer,  0, 0, 0, 255 );
   SDL_RenderFillRect( g_renderer, &area );
#endif /* DEBUG_CGA_EMU */

   return 1;
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

/*
 * @return 1 if blit was successful and 0 otherwise.
 */
int graphics_platform_blit_at(
   const struct GRAPHICS_BITMAP* bmp,
   uint16_t x, uint16_t y, uint16_t w, uint16_t h
) {
   SDL_Rect dest_rect = {
      x * SCREEN_SCALE, 
      y * SCREEN_SCALE,
      w * SCREEN_SCALE, 
      h * SCREEN_SCALE};
   if( NULL == bmp || NULL == bmp->texture ) {
      return 0;
   }

   debug_printf( 1, "blitting resource #%d to %d, %d x %d, %d...",
      bmp->id, x, y, w, h );
   SDL_RenderCopy( g_renderer, bmp->texture, NULL, &dest_rect );

   return 1;
}

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
int32_t graphics_load_bitmap( uint32_t id_in, struct GRAPHICS_BITMAP* b ) {
   uint8_t* buffer = NULL;
   int32_t buffer_sz = 0;
   uint32_t id = 0;
   SDL_RWops* bmp_stream;
   struct DIO_STREAM rstream;
   union DRC_TYPE bitmap_type = DRC_BITMAP_TYPE;

   assert( NULL != b );
   assert( 0 == b->ref_count );

   if( 0 < id_in ) {
      id = id_in;
   } else {
      id = b->id;
   }

   /* Load resource into buffer. */
   memset( &rstream, '\0', sizeof( struct DIO_STREAM ) );
   dio_open_stream_file( DRC_ARCHIVE, "rb", &rstream );
   buffer_sz = drc_get_resource_sz( &rstream, bitmap_type, id );
   buffer = memory_alloc( 1, buffer_sz );
   buffer_sz = drc_get_resource( &rstream, bitmap_type, id, buffer, 0 );
   if( 0 >= buffer_sz ) {
      assert( NULL == buffer );
      return buffer_sz;
   }

   debug_printf( 2, "loaded %d bytes", buffer_sz );

   /* Parse buffered resource into SDL. */
   bmp_stream = SDL_RWFromMem( buffer, buffer_sz );
   b->surface = SDL_LoadBMP_RW( bmp_stream, 1 ); /* Free stream on close. */
   if( NULL == b->surface ) {
      error_printf( "unable to load bitmap %u: %s", id, SDL_GetError() );
      buffer_sz = -1;
      goto cleanup;
   }
   debug_printf( 2, "loaded surface for bitmap resource #%d", id );
   b->texture = SDL_CreateTextureFromSurface( g_renderer, b->surface );
   if( NULL == b->texture ) {
      error_printf( "unable to load texture %u: %s", id, SDL_GetError() );
      buffer_sz = -1;
      if( NULL != b->surface ) {
         SDL_FreeSurface( b->surface );
         b->surface = NULL;
      }
      goto cleanup;
   }
   debug_printf( 2, "loaded texture for bitmap resource #%d", id );

   b->ref_count++;
   b->initialized = 1;

cleanup:

   if( NULL != buffer ) {
      memory_free( &buffer ); /* Free resource memory. */
   }

   if( 0 != dio_type_stream( &rstream ) ) {
      dio_close_stream( &rstream );
   }

   return buffer_sz;
}

/*
 * @return 1 if bitmap is unloaded and 0 otherwise.
 */
int32_t graphics_unload_bitmap( struct GRAPHICS_BITMAP* b ) {
   if( NULL == b ) {
      return 0;
   }
   b->ref_count--;
   if( 0 >= b->ref_count ) {
      debug_printf( 2, "unloading texture and surface for bitmap resource %d",
         b->id );
      SDL_DestroyTexture( b->texture );
      SDL_FreeSurface( b->surface );
      b->initialized = 0;
      return 1;
   }
   return 0;
}

