
#define MENU_C
#include "dsekai.h"

RES_CONST char* gc_menu_msgs[] = {
   "no items"
};

RES_CONST char* gc_menu_item_sel_msgs[] = {
   "use",
   "craft",
   "drop",
   ""
};

void menu_renderer_main( struct DSEKAI_STATE* state ) {
   int8_t i = 1; /* Entry 0 is the main menu, itself. */
   GRAPHICS_COLOR color;
   uint8_t flags = 0;

   window_push(
      MENU_WINDOW_ID, 0, WINDOW_TYPE_WINDOW,
      0,
      SCREEN_MAP_X, SCREEN_MAP_Y, SCREEN_MAP_W, SCREEN_MAP_H,
      WINDOW_PREFAB_DEFAULT_FG(), WINDOW_PREFAB_DEFAULT_BG(), 0,
      0, NULL );
   
   while( '\0' != gc_menu_tokens[i][0] ) {
      if( state->menu.highlight_id == i ) {
         color = WINDOW_PREFAB_DEFAULT_HL();
         flags = GRAPHICS_STRING_FLAG_ALL_CAPS | GRAPHICS_STRING_FLAG_OUTLINE;
      } else {
         color = WINDOW_PREFAB_DEFAULT_FG();
         flags = GRAPHICS_STRING_FLAG_ALL_CAPS;
      }

      window_push(
         WINDOW_ID_MENU_LABEL_ITEM + i, MENU_WINDOW_ID, WINDOW_TYPE_LABEL, 0,
         WINDOW_PADDING_OUTSIDE, WINDOW_PLACEMENT_GRID_RIGHT_DOWN,
         WINDOW_PLACEMENT_CENTER, WINDOW_PLACEMENT_CENTER,
         color, WINDOW_PREFAB_DEFAULT_BG(), flags,
         0, gc_menu_tokens[i] );
      i++;
   }  
}

int16_t menu_handler_main(
   INPUT_VAL in_char, int16_t click_x, int16_t click_y,
   struct DSEKAI_STATE* state
) {
   int16_t retval = 1;

   if( g_input_key_up == in_char ) {
      if( 1 < state->menu.highlight_id ) {
         state->menu.highlight_id--;
      }

   } else if( g_input_key_down == in_char ) {
      if( '\0' != gc_menu_tokens[state->menu.highlight_id + 1][0] ) {
         state->menu.highlight_id++;
      }

   } else if( g_input_key_ok == in_char ) {
      /* TODO: Use quit callback. */
      if( state->menu.highlight_id == gc_menu_idx_quit ) {
#ifndef NO_TITLE
         engines_exit_to_title( state );
#else
         retval = 0;
#endif /* !NO_TITLE */
         menu_close( state );
      } else {
         state->menu.menu_id = state->menu.highlight_id;
         state->menu.highlight_id = 0;
      }

   } else if( g_input_key_menu == in_char ) {
      menu_close( state );
      goto skip_refresh;
   }

   window_pop( MENU_WINDOW_ID );
   state->menu.flags |= MENU_FLAG_DIRTY;

skip_refresh:

   return retval;
}

void menu_renderer_items( struct DSEKAI_STATE* state ) {
   int16_t i = 0,
      player_item_idx = 0;
   GRAPHICS_COLOR color;
   uint8_t flags = 0;
   struct ITEM* items = NULL;

   items = (struct ITEM*)memory_lock( state->items_handle );
   assert( NULL != items );

   if( 0 != (MENU_FLAG_ITEM_OPEN_SEL_MASK & state->menu.flags) ) {
      /* Render the implicit use/craft/drop menu. */
      window_push(
         MENU_WINDOW_ITEM_SEL_ID, 0, WINDOW_TYPE_WINDOW, 0,
         SCREEN_MAP_X + (SCREEN_MAP_W / 2) + TILE_W,
         SCREEN_MAP_Y + (SCREEN_MAP_H / 2),
         (SCREEN_MAP_W / 2) - TILE_W,
         SCREEN_MAP_H / 2,
         WINDOW_PREFAB_DEFAULT_FG(), WINDOW_PREFAB_DEFAULT_BG(), 0,
         0, NULL );

      /* This relies on the item_open_sel_mask being 0-3. */
      for( i = 0 ; i < 4 ; i++ ) {
         /* Flags start from 0x01, since 0x00 is menu closed. */
         if( i == ((MENU_FLAG_ITEM_OPEN_SEL_MASK & state->menu.flags) - 1) ) {
            color = WINDOW_PREFAB_DEFAULT_HL();
            flags = 
               GRAPHICS_STRING_FLAG_ALL_CAPS | GRAPHICS_STRING_FLAG_OUTLINE;
         } else {
            color = WINDOW_PREFAB_DEFAULT_FG();
            flags = GRAPHICS_STRING_FLAG_ALL_CAPS;
         }

         window_push(
            WINDOW_ID_MENU_LABEL_ITEM_SEL_USE + i, MENU_WINDOW_ITEM_SEL_ID,
            WINDOW_TYPE_LABEL, 0,
            WINDOW_PADDING_OUTSIDE, WINDOW_PLACEMENT_GRID_RIGHT_DOWN,
            WINDOW_PLACEMENT_CENTER, WINDOW_PLACEMENT_CENTER,
            color, WINDOW_PREFAB_DEFAULT_BG(), flags,
            0, gc_menu_item_sel_msgs[i] );
      }
   }

   window_push(
      MENU_WINDOW_ID, 0, WINDOW_TYPE_WINDOW, 0,
      SCREEN_MAP_X,
      SCREEN_MAP_Y,
      (SCREEN_MAP_W / 2) + TILE_W,
      SCREEN_MAP_H,
      WINDOW_PREFAB_DEFAULT_FG(), WINDOW_PREFAB_DEFAULT_BG(), 0,
      0, NULL );

   window_push(
      MENU_WINDOW_INFO_ID, 0, WINDOW_TYPE_WINDOW, 0,
      SCREEN_MAP_X + (SCREEN_MAP_W / 2) + TILE_W,
      SCREEN_MAP_Y,
      (SCREEN_MAP_W / 2) - TILE_W,
      SCREEN_MAP_H / 2,
      WINDOW_PREFAB_DEFAULT_FG(), WINDOW_PREFAB_DEFAULT_BG(), 0,
      0, NULL );
   
   /*
   window_push(
      MENU_WINDOW_STATUS_ID, 0, WINDOW_TYPE_WINDOW, 0,
      SCREEN_MAP_X + (SCREEN_MAP_W / 2) + TILE_W,
      SCREEN_MAP_Y + (SCREEN_MAP_H / 2),
      (SCREEN_MAP_W / 2) - TILE_W,
      SCREEN_MAP_H / 2,
      WINDOW_PREFAB_DEFAULT_FG(), WINDOW_PREFAB_DEFAULT_BG(), 0,
      0, 0, NULL,
      state, NULL );
   */
   
   for( i = 0 ; DSEKAI_ITEMS_MAX > i ; i++ ) {
      /* Skip non-player-held or deleted items. */
      /* TODO: Maybe consolidate this with the section to breakout in the
       *       items input handler.
       */
      if(
         ITEM_FLAG_ACTIVE != (ITEM_FLAG_ACTIVE & items[i].flags) ||
         ITEM_OWNER_PLAYER != items[i].owner
      ) {
         continue;
      }

      /* Make sure item sprite is loaded. */
      if( -1 == items[i].sprite_id ) {
         items[i].sprite_id = graphics_cache_load_bitmap(
            items[i].sprite, GRAPHICS_BMP_FLAG_TYPE_SPRITE );
      }

      /* Highlight selected item. */
      if( state->menu.highlight_id == player_item_idx ) {
         color = WINDOW_PREFAB_DEFAULT_HL();
         flags = GRAPHICS_STRING_FLAG_OUTLINE;

         assert( 0 <= items[i].sprite_id );

         /* Show item icon in info window. */
         window_push(
            MENU_WINDOW_ITEM_ICON, MENU_WINDOW_INFO_ID, WINDOW_TYPE_SPRITE,
            0,
            WINDOW_PLACEMENT_CENTER, 6,
            WINDOW_SIZE_AUTO, WINDOW_SIZE_AUTO,
            WINDOW_PREFAB_DEFAULT_FG(), WINDOW_PREFAB_DEFAULT_BG(),
            0, items[i].sprite_id, NULL );
      } else {
         color = WINDOW_PREFAB_DEFAULT_FG();
         flags = 0;
      }

      /* item name */
      window_push(
         WINDOW_ID_MENU_LABEL_ITEM + i, MENU_WINDOW_ID,
         WINDOW_TYPE_LABEL, 0,
         WINDOW_PADDING_OUTSIDE, WINDOW_PLACEMENT_GRID_RIGHT_DOWN,
         WINDOW_PLACEMENT_CENTER, WINDOW_PLACEMENT_CENTER,
         color, WINDOW_PREFAB_DEFAULT_BG(),
         flags | GRAPHICS_STRING_FLAG_ALL_CAPS,
         0, items[i].name );

      /* item count. */
      window_push(
         WINDOW_ID_MENU_LABEL_ITEM + 50 + i, MENU_WINDOW_ID,
         WINDOW_TYPE_LABEL, 0,
         WINDOW_PLACEMENT_RIGHT_BOTTOM, WINDOW_PLACEMENT_GRID,
         WINDOW_PLACEMENT_CENTER, WINDOW_PLACEMENT_CENTER,
         color, WINDOW_PREFAB_DEFAULT_BG(), flags,
         item_get_count_flag( &(items[i]) ), NULL );

      player_item_idx++;
   }  

   if( 0 == player_item_idx ) {
      /* No items were pushed. */
      /* TODO
      control_push(
         0x2320,
         CONTROL_TYPE_LABEL, CONTROL_FLAG_ENABLED | CONTROL_FLAG_TEXT_PTR,
         WINDOW_PADDING_OUTSIDE, WINDOW_PLACEMENT_GRID_RIGHT_DOWN,
         WINDOW_PLACEMENT_CENTER, WINDOW_PLACEMENT_CENTER,
         WINDOW_PREFAB_DEFAULT_FG(), WINDOW_PREFAB_DEFAULT_BG(),
         GRAPHICS_STRING_FLAG_ALL_CAPS,
         0, 0, gc_menu_msgs[0], MENU_WINDOW_ID, state ); */
   }

   items = (struct ITEM*)memory_unlock( state->items_handle );
}

static int8_t menu_handler_items_use(
   int16_t selected_item_idx, struct DSEKAI_STATE* state
) {
   int8_t use_retval = 0;
   uint8_t item_type = 0;
   struct ITEM* items = NULL;

   items = (struct ITEM*)memory_lock( state->items_handle );
   item_type = item_get_type_flag( &(items[selected_item_idx]) );
   items = (struct ITEM*)memory_unlock( state->items_handle );

   use_retval = gc_item_use_cbs[item_type](
      selected_item_idx, ITEM_OWNER_PLAYER, state );
   if( ITEM_USED_SUCCESSFUL_SILENT != use_retval ) {
      menu_close( state );
   }

   return use_retval;
}

int16_t menu_handler_items(
   INPUT_VAL in_char, int16_t click_x, int16_t click_y,
   struct DSEKAI_STATE* state
) {
   int16_t retval = 1,
      i = 0;
   int16_t selected_item_idx = ITEM_ERROR_NOT_FOUND,
      player_item_count = 0;
   struct TILEMAP* t = NULL;
   struct ITEM* items = NULL;

   /* TODO: Break this first part out into its own function. */

   /* Handle locking. */
   items = (struct ITEM*)memory_lock( state->items_handle );
   assert( NULL != items );

   /* Count player-owned items to enforce limits below. */
   for( i = 0 ; DSEKAI_ITEMS_MAX > i ; i++ ) {
      if(
         ITEM_OWNER_PLAYER == items[i].owner &&
         ITEM_FLAG_ACTIVE == (ITEM_FLAG_ACTIVE & items[i].flags)
      ) {
         if( player_item_count == state->menu.highlight_id ) {
            /* This is the currently selected item. */
            assert( ITEM_ERROR_NOT_FOUND == selected_item_idx );
            selected_item_idx = i;
         }
         player_item_count++;
      }
   }

   items = (struct ITEM*)memory_unlock( state->items_handle );

   if( g_input_key_up == in_char ) {
      /* Process the implicity use/craft/drop menu first if that's open. */
      if(
         MENU_FLAG_ITEM_OPEN_SEL_USE ==
         (state->menu.flags & MENU_FLAG_ITEM_OPEN_SEL_MASK)
      ) {
         state->menu.flags &= ~MENU_FLAG_ITEM_OPEN_SEL_MASK;
         state->menu.flags |= MENU_FLAG_ITEM_OPEN_SEL_DROP;

      } else if(
         MENU_FLAG_ITEM_OPEN_SEL_CRAFT ==
         (state->menu.flags & MENU_FLAG_ITEM_OPEN_SEL_MASK)
      ) {
         state->menu.flags &= ~MENU_FLAG_ITEM_OPEN_SEL_MASK;
         state->menu.flags |= MENU_FLAG_ITEM_OPEN_SEL_USE;

      } else if(
         MENU_FLAG_ITEM_OPEN_SEL_DROP ==
         (state->menu.flags & MENU_FLAG_ITEM_OPEN_SEL_MASK)
      ) {
         state->menu.flags &= ~MENU_FLAG_ITEM_OPEN_SEL_MASK;
         state->menu.flags |= MENU_FLAG_ITEM_OPEN_SEL_CRAFT;
      
      } else if( 0 < state->menu.highlight_id ) {
         /* The use/craft/drop menu isn't open, so iterate items. */
         state->menu.highlight_id--;
      }

   } else if( g_input_key_down == in_char ) {
      /* Process the implicity use/craft/drop menu first if that's open. */
      if(
         MENU_FLAG_ITEM_OPEN_SEL_USE ==
         (state->menu.flags & MENU_FLAG_ITEM_OPEN_SEL_MASK)
      ) {
         state->menu.flags &= ~MENU_FLAG_ITEM_OPEN_SEL_MASK;
         state->menu.flags |= MENU_FLAG_ITEM_OPEN_SEL_CRAFT;

      } else if(
         MENU_FLAG_ITEM_OPEN_SEL_CRAFT ==
         (state->menu.flags & MENU_FLAG_ITEM_OPEN_SEL_MASK)
      ) {
         state->menu.flags &= ~MENU_FLAG_ITEM_OPEN_SEL_MASK;
         state->menu.flags |= MENU_FLAG_ITEM_OPEN_SEL_DROP;

      } else if(
         MENU_FLAG_ITEM_OPEN_SEL_DROP ==
         (state->menu.flags & MENU_FLAG_ITEM_OPEN_SEL_MASK)
      ) {
         state->menu.flags &= ~MENU_FLAG_ITEM_OPEN_SEL_MASK;
         state->menu.flags |= MENU_FLAG_ITEM_OPEN_SEL_USE;

      } else if( state->menu.highlight_id + 1 < player_item_count ) {
         /* The use/craft/drop menu isn't open, so iterate items. */
         state->menu.highlight_id++;
      }

   } else if( g_input_key_ok == in_char ) {
      if( ITEM_ERROR_NOT_FOUND == selected_item_idx ) {
         error_printf( "no item selected!" );
         goto not_found;
      }

#if 0
      if(
         MENU_FLAG_ITEM_CRAFTABLE ==
         (state->menu.flags & MENU_FLAG_ITEM_CRAFTABLE)
      ) {
         /* TODO */
      }
#endif

      if(
         MENU_FLAG_ITEM_OPEN_SEL_USE ==
         (state->menu.flags & MENU_FLAG_ITEM_OPEN_SEL_MASK)
      ) {
         menu_handler_items_use( selected_item_idx, state );
         state->menu.flags &= ~MENU_FLAG_ITEM_OPEN_SEL_MASK;

      } else if(
         MENU_FLAG_ITEM_OPEN_SEL_CRAFT ==
         (state->menu.flags & MENU_FLAG_ITEM_OPEN_SEL_MASK)
      ) {
         /* TODO */
         state->menu.flags &= ~MENU_FLAG_ITEM_OPEN_SEL_MASK;

      } else if(
         MENU_FLAG_ITEM_OPEN_SEL_DROP ==
         (state->menu.flags & MENU_FLAG_ITEM_OPEN_SEL_MASK)
      ) {
         t = (struct TILEMAP*)memory_lock( state->map_handle );
         if( NULL == t ) {
            error_printf( "could not lock tilemap for dropping!" );
            retval = 0;
            goto cleanup;
         }
         item_drop( selected_item_idx, t, state );
         t = (struct TILEMAP*)memory_unlock( state->map_handle );
         state->menu.flags &= ~MENU_FLAG_ITEM_OPEN_SEL_MASK;

      } else {
         /* Open the implicit use/craft/drop menu. */
         state->menu.flags |= MENU_FLAG_ITEM_OPEN_SEL_USE;
      }

   } else if( g_input_key_menu == in_char ) {
      if( 0 != (state->menu.flags & MENU_FLAG_ITEM_OPEN_SEL_MASK) ) {
         /* Close the implicit use/craft/drop menu. */
         state->menu.flags &= ~MENU_FLAG_ITEM_OPEN_SEL_MASK;
      } else {
         /* Close the items menu. */
         state->menu.menu_id = 0;
         state->menu.highlight_id = 1;
      }
   }

not_found:

   /* Close all item menu windows to and refresh. */
   window_pop( MENU_WINDOW_ID );
   window_pop( MENU_WINDOW_INFO_ID );
   /* window_pop( MENU_WINDOW_STATUS_ID ); */
   window_pop( MENU_WINDOW_ITEM_SEL_ID );

   state->menu.flags |= MENU_FLAG_DIRTY;

cleanup:

   return retval;
}

void menu_renderer_craft( struct DSEKAI_STATE* state ) {

   window_push(
      MENU_WINDOW_CRAFT_ID, 0, WINDOW_TYPE_WINDOW, 0,
      WINDOW_PLACEMENT_CENTER,
      WINDOW_PLACEMENT_CENTER,
      SCREEN_MAP_W / 2,
      SCREEN_MAP_H / 2,
      WINDOW_PREFAB_DEFAULT_FG(), WINDOW_PREFAB_DEFAULT_BG(), 0,
      0, NULL );

}

int16_t menu_handler_craft(
   INPUT_VAL in_char, int16_t click_x, int16_t click_y,
   struct DSEKAI_STATE* state
) {

   state->menu.menu_id = 0;
   state->menu.highlight_id = 1;

   return 1;
}

void menu_renderer_save( struct DSEKAI_STATE* state ) {

#ifdef TILEMAP_FMT_ASN

   /* TODO: Enter name for save. */
   serial_save( "save.asn", state );

#endif /* TILEMAP_FMT_ASN */

   menu_close( state );
}

int16_t menu_handler_save(
   INPUT_VAL in_char, int16_t click_x, int16_t click_y,
   struct DSEKAI_STATE* state
) {
   return 1;
}

void menu_renderer_quit( struct DSEKAI_STATE* state ) {
   /* Nothing to render. */
}

int16_t menu_handler_quit(
   INPUT_VAL in_char, int16_t click_x, int16_t click_y,
   struct DSEKAI_STATE* state
) {
   return 0;
}

void menu_open( struct DSEKAI_STATE* state ) {
   debug_printf( 3, "opening menu..." );

   state->menu.menu_id = 0;
   state->menu.highlight_id = 1;
   state->menu.flags |= MENU_FLAG_DIRTY;
   state->menu.open_ms = graphics_get_ms();

   animate_pause( ANIMATE_FLAG_SCRIPT );
   animate_pause( ANIMATE_FLAG_WEATHER );
}

void menu_close( struct DSEKAI_STATE* state ) {
   int8_t i = 0;
   uint32_t open_ms_diff = 0;
   struct TILEMAP* map = NULL;

   debug_printf( 3, "closing menu..." );
   window_pop( MENU_WINDOW_ID );
   state->menu.menu_id = -1;
   state->menu.highlight_id = -1;
   map = (struct TILEMAP*)memory_lock( state->map_handle );
   if( NULL != map ) {
      tilemap_refresh_tiles( map );
      map = (struct TILEMAP*)memory_unlock( state->map_handle );
   }

   open_ms_diff = graphics_get_ms() - state->menu.open_ms;

   /* Bump up all crop cycles to compensate for menu time. */
   for( i = 0 ; DSEKAI_CROPS_MAX > i ; i++ ) {
      if(
         CROP_FLAG_ACTIVE != (CROP_FLAG_ACTIVE & state->crops[i].flags) ||
         0 == state->crops[i].crop_gid
      ) {
         continue;
      }

      debug_printf( 1, "crop %d next cycle bumped up %d ms from %d to %d",
         state->crops[i].crop_gid,
         open_ms_diff,
         state->crops[i].next_at_ticks,
         state->crops[i].next_at_ticks + open_ms_diff );

      state->crops[i].next_at_ticks += open_ms_diff;
   }

   animate_resume( ANIMATE_FLAG_SCRIPT );
   animate_resume( ANIMATE_FLAG_WEATHER );
}

