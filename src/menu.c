
#define MENU_C
#include "dsekai.h"

void menu_renderer_main( struct DSEKAI_STATE* state ) {
   int8_t i = 1; /* Entry 0 is the main menu, itself. */
   GRAPHICS_COLOR color;

   window_push(
      MENU_WINDOW_ID, WINDOW_STATUS_MODAL,
      SCREEN_MAP_X, SCREEN_MAP_Y, SCREEN_MAP_W, SCREEN_MAP_H,
      0, state );
   
   while( '\0' != gc_menu_tokens[i][0] ) {
      if( state->menu.highlight_id == i ) {
         color = GRAPHICS_COLOR_CYAN;
      } else {
         color = GRAPHICS_COLOR_WHITE;
      }

      control_push(
         0x2323,
         CONTROL_TYPE_LABEL, CONTROL_FLAG_ENABLED | CONTROL_FLAG_TEXT_MENU,
         10, CONTROL_PLACEMENT_GRID_DOWN,
         CONTROL_PLACEMENT_CENTER, CONTROL_PLACEMENT_CENTER,
         color, GRAPHICS_COLOR_BLACK,
         GRAPHICS_STRING_FLAGS_ALL_CAPS,
         i, 0, MENU_WINDOW_ID, state );
      i++;
   }  
}

int16_t menu_handler_main( char in_char, struct DSEKAI_STATE* state ) {
   int16_t retval = 1;

   switch( in_char ) {
   case INPUT_KEY_UP:
      if( 1 < state->menu.highlight_id ) {
         state->menu.highlight_id--;
      }
      window_pop( MENU_WINDOW_ID, state );
      state->menu.flags |= MENU_FLAG_DIRTY;
      break;

   case INPUT_KEY_DOWN:
      if( '\0' != gc_menu_tokens[state->menu.highlight_id + 1][0] ) {
         state->menu.highlight_id++;
      }
      window_pop( MENU_WINDOW_ID, state );
      state->menu.flags |= MENU_FLAG_DIRTY;
      break;

   case INPUT_KEY_OK:
      /* TODO: Use quit callback. */
      if( state->menu.highlight_id == 2 /* quit */ ) {
         retval = 0;
      }
      break;

   case INPUT_KEY_QUIT:
      menu_close( state );
      break;
   }

   return retval;
}

void menu_renderer_items( struct DSEKAI_STATE* state ) {

}

int16_t menu_handler_items( char in_char, struct DSEKAI_STATE* state ) {
   int16_t retval = 1;

   return retval;
}

void menu_renderer_quit( struct DSEKAI_STATE* state ) {
   /* Nothing to render. */
}

int16_t menu_handler_quit( char in_char, struct DSEKAI_STATE* state ) {
   return 0;
}

void menu_open( struct DSEKAI_STATE* state ) {
   state->menu.menu_id = 0;
   state->menu.highlight_id = 1;
   state->menu.flags |= MENU_FLAG_DIRTY;

   animate_pause( ANIMATE_FLAG_SCRIPT );
   animate_pause( ANIMATE_FLAG_WEATHER );
}

void menu_close( struct DSEKAI_STATE* state ) {
   window_pop( MENU_WINDOW_ID, state );
   state->menu.menu_id = -1;
   state->menu.highlight_id = -1;
   tilemap_refresh_tiles( &(state->map) );

   animate_resume( ANIMATE_FLAG_SCRIPT );
   animate_resume( ANIMATE_FLAG_WEATHER );
}

