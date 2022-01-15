
#define MENU_C
#include "dsekai.h"

const char* gc_menu_msgs[] = {
   "no items"
};

void menu_renderer_main( struct DSEKAI_STATE* state ) {
   int8_t i = 1; /* Entry 0 is the main menu, itself. */
   GRAPHICS_COLOR color;

   window_push(
      MENU_WINDOW_ID, 0, WINDOW_TYPE_WINDOW,
      0,
      SCREEN_MAP_X, SCREEN_MAP_Y, SCREEN_MAP_W, SCREEN_MAP_H,
      GRAPHICS_COLOR_WHITE, GRAPHICS_COLOR_BLACK, 0,
      0, 0, NULL, state );
   
   while( '\0' != gc_menu_tokens[i][0] ) {
      if( state->menu.highlight_id == i ) {
         color = GRAPHICS_COLOR_CYAN;
      } else {
         color = GRAPHICS_COLOR_WHITE;
      }

      window_push(
         WINDOW_ID_MENU_LABEL_ITEM + i, MENU_WINDOW_ID, WINDOW_TYPE_LABEL,
         WINDOW_FLAG_TEXT_MENU,
         10, WINDOW_PLACEMENT_GRID_RIGHT_DOWN,
         WINDOW_PLACEMENT_CENTER, WINDOW_PLACEMENT_CENTER,
         color, GRAPHICS_COLOR_BLACK, GRAPHICS_STRING_FLAG_ALL_CAPS,
         i, 0, NULL, state );
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
      break;

   case INPUT_KEY_DOWN:
      if( '\0' != gc_menu_tokens[state->menu.highlight_id + 1][0] ) {
         state->menu.highlight_id++;
      }
      break;

   case INPUT_KEY_OK:
      /* TODO: Use quit callback. */
      if( state->menu.highlight_id == gc_menu_idx_quit ) {
         /* retval = 0; */
         engines_exit_to_title( state );
         menu_close( state );
      } else {
         state->menu.menu_id = state->menu.highlight_id;
         state->menu.highlight_id = 0;
      }
      break;

   case INPUT_KEY_QUIT:
      menu_close( state );
      goto skip_refresh;
   }

   window_pop( MENU_WINDOW_ID, state );
   state->menu.flags |= MENU_FLAG_DIRTY;

skip_refresh:

   return retval;
}

void menu_renderer_items( struct DSEKAI_STATE* state ) {
   int8_t i = 0,
      player_item_idx = 0;
   GRAPHICS_COLOR color;

   window_push(
      MENU_WINDOW_ID, 0, WINDOW_TYPE_WINDOW, 0,
      SCREEN_MAP_X,
      SCREEN_MAP_Y,
      (SCREEN_MAP_W / 2) + TILE_W,
      SCREEN_MAP_H,
      GRAPHICS_COLOR_WHITE, GRAPHICS_COLOR_BLACK, 0,
      0, 0, NULL,
      state );

   window_push(
      MENU_WINDOW_INFO_ID, 0, WINDOW_TYPE_WINDOW, 0,
      SCREEN_MAP_X + (SCREEN_MAP_W / 2) + TILE_W,
      SCREEN_MAP_Y,
      (SCREEN_MAP_W / 2) - TILE_W,
      SCREEN_MAP_H / 2,
      GRAPHICS_COLOR_WHITE, GRAPHICS_COLOR_BLACK, 0,
      0, 0, NULL,
      state );
   
   window_push(
      MENU_WINDOW_STATUS_ID, 0, WINDOW_TYPE_WINDOW, 0,
      SCREEN_MAP_X + (SCREEN_MAP_W / 2) + TILE_W,
      SCREEN_MAP_Y + (SCREEN_MAP_H / 2),
      (SCREEN_MAP_W / 2) - TILE_W,
      SCREEN_MAP_H / 2,
      GRAPHICS_COLOR_WHITE, GRAPHICS_COLOR_BLACK, 0,
      0, 0, NULL,
      state );
   
   for( i = 0 ; DSEKAI_ITEMS_MAX > i ; i++ ) {
      /* Skip non-player-held or deleted items. */
      if(
         ITEM_OWNER_PLAYER != state->items[i].owner ||
         ITEM_FLAG_ACTIVE != (ITEM_FLAG_ACTIVE & state->items[i].flags)
      ) {
         continue;
      }

      /* Highlight selected item. */
      if( state->menu.highlight_id == player_item_idx ) {
         color = GRAPHICS_COLOR_CYAN;
      } else {
         color = GRAPHICS_COLOR_WHITE;
      }

      /* TODO: Draw icons for item types/flags. */

      window_push(
         WINDOW_ID_MENU_LABEL_ITEM + i, MENU_WINDOW_ID,
         WINDOW_TYPE_LABEL, WINDOW_FLAG_TEXT_ITEM,
         10, WINDOW_PLACEMENT_GRID_RIGHT_DOWN,
         WINDOW_PLACEMENT_CENTER, WINDOW_PLACEMENT_CENTER,
         color, GRAPHICS_COLOR_BLACK, GRAPHICS_STRING_FLAG_ALL_CAPS,
         i, 0, NULL, state );

      window_push(
         WINDOW_ID_MENU_LABEL_ITEM + 100 + i, MENU_WINDOW_ID,
         WINDOW_TYPE_LABEL, WINDOW_FLAG_TEXT_NUM,
         WINDOW_PLACEMENT_RIGHT_BOTTOM, WINDOW_PLACEMENT_GRID,
         WINDOW_PLACEMENT_CENTER, WINDOW_PLACEMENT_CENTER,
         color, GRAPHICS_COLOR_BLACK, 0,
         state->items[i].count, 0, NULL, state );

      player_item_idx++;
   }  

   if( 0 == player_item_idx ) {
      /* No items were pushed. */
      /* TODO
      control_push(
         0x2320,
         CONTROL_TYPE_LABEL, CONTROL_FLAG_ENABLED | CONTROL_FLAG_TEXT_PTR,
         10, WINDOW_PLACEMENT_GRID_RIGHT_DOWN,
         WINDOW_PLACEMENT_CENTER, WINDOW_PLACEMENT_CENTER,
         GRAPHICS_COLOR_WHITE, GRAPHICS_COLOR_BLACK,
         GRAPHICS_STRING_FLAG_ALL_CAPS,
         0, 0, gc_menu_msgs[0], MENU_WINDOW_ID, state ); */
   }
}

int16_t menu_handler_items( char in_char, struct DSEKAI_STATE* state ) {
   int16_t retval = 1,
      i = 0,
      player_item_count = 0;
   int8_t use_retval = 0;
   struct ITEM* selected_item = NULL;

   /* Count player-owned items to enforce limits below. */
   for( i = 0 ; DSEKAI_ITEMS_MAX > i ; i++ ) {
      if(
         ITEM_OWNER_PLAYER == state->items[i].owner &&
         ITEM_FLAG_ACTIVE == (ITEM_FLAG_ACTIVE & state->items[i].flags)
      ) {
         if( player_item_count == state->menu.highlight_id ) {
            /* This is the currently selected item. */
            selected_item = &(state->items[i]);
         }
         player_item_count++;
      }
   }

   switch( in_char ) {
   case INPUT_KEY_UP:
      if( 0 < state->menu.highlight_id ) {
         state->menu.highlight_id--;
      }
      break;

   case INPUT_KEY_DOWN:
      if( state->menu.highlight_id + 1 < player_item_count ) {
         state->menu.highlight_id++;
      }
      break;

   case INPUT_KEY_OK:
      /* TODO: Use item. */
      if( NULL == selected_item ) {
         error_printf( "no item selected!" );
         break;
      }
      if(
         MENU_FLAG_ITEM_CRAFTABLE ==
         (state->menu.flags & MENU_FLAG_ITEM_CRAFTABLE)
      ) {
         /* TODO: Open crafting menu. */
      } else {
         use_retval = gc_item_use_cbs[selected_item->type](
            selected_item, &(state->player), state );
         if( 0 > use_retval ) {
            menu_close( state );
         }
      }
      break;

   case INPUT_KEY_QUIT:
      state->menu.menu_id = 0;
      state->menu.highlight_id = 1;
      break;
   }

   /* Close all item menu windows to and refresh. */
   window_pop( MENU_WINDOW_ID, state );
   window_pop( MENU_WINDOW_INFO_ID, state );
   window_pop( MENU_WINDOW_STATUS_ID, state );
   state->menu.flags |= MENU_FLAG_DIRTY;

   return retval;
}

void menu_renderer_craft( struct DSEKAI_STATE* state ) {

   window_push(
      MENU_WINDOW_CRAFT_ID, 0, WINDOW_TYPE_WINDOW, 0,
      WINDOW_PLACEMENT_CENTER,
      WINDOW_PLACEMENT_CENTER,
      SCREEN_MAP_W / 2,
      SCREEN_MAP_H / 2,
      GRAPHICS_COLOR_WHITE, GRAPHICS_COLOR_BLACK, 0,
      0, 0, NULL,
      state );

}

int16_t menu_handler_craft( char in_char, struct DSEKAI_STATE* state ) {

   state->menu.menu_id = 0;
   state->menu.highlight_id = 1;

   return 1;
}

void menu_renderer_quit( struct DSEKAI_STATE* state ) {
   /* Nothing to render. */
}

int16_t menu_handler_quit( char in_char, struct DSEKAI_STATE* state ) {
   return 0;
}

void menu_open( struct DSEKAI_STATE* state ) {
   debug_printf( 3, "opening menu..." );

   state->menu.menu_id = 0;
   state->menu.highlight_id = 1;
   state->menu.flags |= MENU_FLAG_DIRTY;

   animate_pause( ANIMATE_FLAG_SCRIPT );
   animate_pause( ANIMATE_FLAG_WEATHER );
}

void menu_close( struct DSEKAI_STATE* state ) {
   debug_printf( 3, "closing menu..." );
   window_pop( MENU_WINDOW_ID, state );
   state->menu.menu_id = -1;
   state->menu.highlight_id = -1;
   tilemap_refresh_tiles( &(state->map) );

   animate_resume( ANIMATE_FLAG_SCRIPT );
   animate_resume( ANIMATE_FLAG_WEATHER );
}

