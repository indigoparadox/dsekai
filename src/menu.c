
#define MENU_C
#include "dsekai.h"

void menu_renderer_main( struct DSEKAI_STATE* state ) {
   int8_t i = 1; /* Entry 0 is the main menu, itself. */

   window_push(
      MENU_WINDOW_ID, WINDOW_STATUS_MODAL,
      SCREEN_MAP_X, SCREEN_MAP_Y, SCREEN_MAP_W, SCREEN_MAP_H,
      0, state );
   
   while( '\0' != gc_menu_tokens[i][0] ) {
      /* control_push(
         0x2323,
         CONTROL_TYPE_LABEL_T, CONTROL_STATE_ENABLED,
         10, CONTROL_PLACEMENT_GRID_DOWN,
         CONTROL_PLACEMENT_CENTER, CONTROL_PLACEMENT_CENTER,
         GRAPHICS_COLOR_WHITE, GRAPHICS_COLOR_BLACK,
         1, dialog, 0, id, state, state->map.strings ); */
   }  
}

int16_t menu_handler_main( struct DSEKAI_STATE* state ) {
   int16_t retval = 1;

   return retval;
}

void menu_renderer_items( struct DSEKAI_STATE* state ) {

}

int16_t menu_handler_items( struct DSEKAI_STATE* state ) {
   int16_t retval = 1;

   return retval;
}

void menu_renderer_quit( struct DSEKAI_STATE* state ) {
   /* Nothing to render. */
}

int16_t menu_handler_quit( struct DSEKAI_STATE* state ) {
   return 0;
}

