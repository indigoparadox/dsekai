
#include "dsekai.h"

/**
 * \brief Draw currently on-screen portion of a TILEMAP.
 * \param state ::MEMORY_PTR to the current engine state, used to determine
 *              what is currently on-screen.
 */
static
void topdown_draw_tilemap( struct DSEKAI_STATE* state ) {
   int16_t x = 0,
      y = 0,
      i = 0;
   uint8_t tile_id = 0;
   uint16_t
      tile_px = 0, /* Tile on-screen X in pixels. */
      tile_py = 0, /* Tile on-screen Y in pixels. */
      viewport_tx2 = 0,
      viewport_ty2 = 0;
   struct TOPDOWN_STATE* gstate = NULL;
   struct TILEMAP* map = NULL;

   gstate = (struct TOPDOWN_STATE*)memory_lock( state->engine_state_handle );
   if( NULL == gstate ) {
      error_printf( "could not lock gstate" );
      goto cleanup;
   }

   map = (struct TILEMAP*)memory_lock( state->map_handle );
   if( NULL == map ) {
      error_printf( "could not lock tilemap" );
      goto cleanup;
   }

   /* Tile-indexed rectangle of on-screen tiles. */
   viewport_tx2 = gstate->screen_scroll_tx + SCREEN_TW;
   viewport_ty2 = gstate->screen_scroll_ty + SCREEN_TH;

   assert( viewport_tx2 <= TILEMAP_TW );
   assert( viewport_ty2 <= TILEMAP_TH );

   /* Iterate over only on-screen tiles. */
   for( y = gstate->screen_scroll_ty ; viewport_ty2 > y ; y++ ) {
      for( x = gstate->screen_scroll_tx ; viewport_tx2 > x ; x++ ) {

         tile_px = (x * TILE_W) - gstate->screen_scroll_x;
         tile_py = (y * TILE_H) - gstate->screen_scroll_y;

#ifndef NO_ENGINE_EDITOR

         /* Flash the tile and continue if the editor is active. */
         if(
            EDITOR_FLAG_ACTIVE == (EDITOR_FLAG_ACTIVE & state->editor.flags)
            && x == state->editor.coords.x && y == state->editor.coords.y &&
            0 == state->ani_sprite_x
         ) {
            tilemap_set_dirty( x, y, map );
            
            /* If we're not forcing the frame, black out the block.
             * If we ARE, then just draw as usual after marking dirty above. */
            if(
               EDITOR_FLAG_FORCE_FRAME !=
               (EDITOR_FLAG_FORCE_FRAME & state->editor.flags)
            ) {
               /* Black out the block. */
               graphics_draw_block(
                  SCREEN_MAP_X + tile_px, SCREEN_MAP_Y + tile_py,
                  TILE_W, TILE_H, GRAPHICS_COLOR_BLACK );
               continue;

            } else {
               /* Un-force the next frame. */
               state->editor.flags &= ~EDITOR_FLAG_FORCE_FRAME;
            }
         }

#endif /* !NO_ENGINE_EDITOR */

#ifndef NO_ANIMATE
         /* Mark as dirty any on-screen tiles under an animation. */
         for( i = 0 ; ANIMATE_ANIMATIONS_MAX > i ; i++ ) {

            if(
               ANIMATE_FLAG_ACTIVE ==
                  (g_animations[i].flags & ANIMATE_FLAG_ACTIVE) &&
               (g_animations[i].x - SCREEN_MAP_X)
                  <= tile_px &&
               (g_animations[i].x - SCREEN_MAP_X) + g_animations[i].w
                  > tile_px &&
               (g_animations[i].y - SCREEN_MAP_Y)
                  <= tile_py &&
               (g_animations[i].y - SCREEN_MAP_Y) + g_animations[i].h
                  > tile_py
            ) {
               tilemap_set_dirty( x, y, map );
            }
         }
#endif /* !NO_ANIMATE */

         if( !tilemap_is_dirty( x, y, map ) ) {
            continue;
         }

         /* Sanity checks. */
         assert( y < TILEMAP_TH );
         assert( x < TILEMAP_TW );
         assert( y >= 0 );
         assert( x >= 0 );

         tilemap_unset_dirty( x, y, map );

         /* Grab the left byte if even or the right if odd. */
         tile_id = tilemap_get_tile_id( map, x, y );

         if( tile_id >= TILEMAP_TILESETS_MAX ) {
            error_printf( "invalid tile id: %d", tile_id );
            continue;
         }
         
         assert( 0 <= map->tileset[tile_id].image_id );

         /* Blit the tile. */
         graphics_blit_tile_at(
            map->tileset[tile_id].image_id,
            0, 0,
            SCREEN_MAP_X + tile_px, SCREEN_MAP_Y + tile_py,
            TILE_W, TILE_H );
      }
   }

cleanup:

   if( NULL != map ) {
      map = (struct TILEMAP*)memory_unlock( state->map_handle );
   }

   if( NULL != gstate ) {
      gstate =
         (struct TOPDOWN_STATE*)memory_unlock( state->engine_state_handle );
   }
}

static void topdown_draw_crops(
   struct DSEKAI_STATE* state, struct TOPDOWN_STATE* gstate
) {
   int8_t i = 0,
      crop_idx = 0;
   struct CROP_PLOT* plot = NULL;
   int16_t plot_gfx;
   uint16_t plot_px = 0,
      plot_py = 0;
   uint8_t crop_stage = 0;
   struct TILEMAP* map = NULL;
   struct CROP_DEF* crop_def = NULL;

   map = (struct TILEMAP*)memory_lock( state->map_handle );
   if( NULL == map ) {
      error_printf( "could not lock tilemap" );
      return;
   }

   /* TODO: Use tile dirty bit to avoid redraw unless crop sprite changes. */

   /* Draw crops/plots. */
   for( i = 0 ; DSEKAI_CROPS_MAX > i ; i++ ) {
      plot = &(state->crops[i]);
      if(
         /* Crop plot is inactive. */
         CROP_FLAG_ACTIVE != (plot->flags & CROP_FLAG_ACTIVE) ||
         /* Crop is on a different TILEMAP. */
         0 != memory_strncmp_ptr(
            plot->map_name, map->name,
            memory_strnlen_ptr( plot->map_name, TILEMAP_NAME_MAX ) ) ||
         /* Crop tile is not dirty. */
         /* !tilemap_is_dirty( plot->coords.x, plot->coords.y, map ) || */
         /* Crop is off-screen. */
         plot->coords.x < gstate->screen_scroll_tx ||
            plot->coords.y < gstate->screen_scroll_ty ||
            plot->coords.x >= gstate->screen_scroll_tx + SCREEN_TW ||
            plot->coords.y >= gstate->screen_scroll_ty + SCREEN_TH
      ) {
         continue;
      }

      plot_px =
         SCREEN_MAP_X + ((plot->coords.x * TILE_W) - gstate->screen_scroll_x);
      plot_py =
         SCREEN_MAP_Y + ((plot->coords.y * TILE_H) - gstate->screen_scroll_y);

#ifdef RESOURCE_FILE
      plot_gfx = 
         graphics_cache_load_bitmap( ASSETS_PATH DEPTH_SPEC "/i_plot.bmp" );
#else
      plot_gfx = graphics_cache_load_bitmap( i_plot );
#endif /* RESOURCE_FILE */

      assert( 0 < plot_gfx );

      graphics_blit_sprite_at(
         plot_gfx, 0, 0, plot_px, plot_py, TILE_W, TILE_H );

      /* Skip drawing crop if it hasn't germinated. */
      if(
         0 == plot->crop_gid ||
         0 > (crop_idx = crop_get_def_idx( plot->crop_gid, map ))
      ) {
         continue;
      }

      /* Make sure crop spritesheet is loaded. */
      crop_def = &(map->crop_defs[crop_idx]);
      if( 0 > crop_def->sprite_id ) {
         crop_def->sprite_id = graphics_cache_load_bitmap( crop_def->sprite );
      }

      crop_stage = (plot->flags & CROP_FLAG_STAGE_MASK);

      if( 0 < crop_stage ) {
         /* Crop has germinated. */
         graphics_blit_sprite_at(
            crop_def->sprite_id,
            (crop_stage - 1) * TILE_W, 0,
            plot_px, plot_py, TILE_W, TILE_H );

      } else {
         /* Crop is still seeds. */
#ifdef RESOURCE_FILE
         plot_gfx = graphics_cache_load_bitmap(
            ASSETS_PATH DEPTH_SPEC "/i_seed.bmp" );
#else
         plot_gfx = graphics_cache_load_bitmap( i_seed );
#endif /* RESOURCE_FILE */
         graphics_blit_sprite_at(
            plot_gfx, 0, 0, plot_px, plot_py, TILE_W, TILE_H );
      }
   }

   map = (struct TILEMAP*)memory_unlock( state->map_handle );
}

static void topdown_draw_mobile(
   struct DSEKAI_STATE* state, struct TOPDOWN_STATE* gstate, struct MOBILE* m
) {

   if(
      /* Don't draw inactive mobiles. */
      (MOBILE_FLAG_ACTIVE != (MOBILE_FLAG_ACTIVE & m->flags)) ||
      /* Pre-death blinking effect (skip negative even frames). */
      (0 > m->mp_hp && 0 == m->mp_hp % 2)
   ) {
      return;
   }

   if(
      m->coords.x < gstate->screen_scroll_tx ||
      m->coords.y < gstate->screen_scroll_ty ||
      m->coords.x >= gstate->screen_scroll_tx + SCREEN_TW ||
      m->coords.y >= gstate->screen_scroll_ty + SCREEN_TH
   ) {
      /* Mobile is off-screen. */
      m->screen_px = -1;
      m->screen_py = -1;
      return;
   }

   m->screen_px = 
      SCREEN_MAP_X + (m->coords.x * SPRITE_W) - gstate->screen_scroll_x;
   m->screen_py =
      SCREEN_MAP_Y + (m->coords.y * SPRITE_H) - gstate->screen_scroll_y;

   /* Figure out direction to offset steps in. */
   if( m->coords_prev.x > m->coords.x ) {
      m->screen_px += SPRITE_W - m->steps_x;
   
   } else if( m->coords_prev.x < m->coords.x ) {
      m->screen_px -= SPRITE_W - m->steps_x;

   } else if( m->coords_prev.y > m->coords.y ) {
      m->screen_py += SPRITE_H - m->steps_y;
   
   } else if( m->coords_prev.y < m->coords.y ) {
      m->screen_py -= SPRITE_H - m->steps_y;
   }

   assert( 0 <= m->sprite_id );

   /* Blit the mobile's current sprite/frame. */
   graphics_blit_sprite_at(
      m->sprite_id, state->ani_sprite_x, mobile_get_dir( m ) * SPRITE_H,
      m->screen_px, m->screen_py, SPRITE_W, SPRITE_H );
}

void topdown_draw_items(
   struct DSEKAI_STATE* state, struct TOPDOWN_STATE* gstate
) {
   int8_t i = 0;
   uint16_t item_px = 0,
      item_py = 0;
   struct TILEMAP* map = NULL;
   struct ITEM* items = NULL;

   items = (struct ITEM*)memory_lock( state->items_handle );
   assert( NULL != items );
   if( NULL == items ) {
      error_printf( "could not lock items" );
      goto cleanup;
   }

   map = (struct TILEMAP*)memory_lock( state->map_handle );
   if( NULL == map ) {
      error_printf( "could not lock tilemap" );
      goto cleanup;
   }

   for( i = 0 ; DSEKAI_ITEMS_MAX > i ; i++ ) {
      if(
         /* Item is inactive. */
         !(items[i].flags & ITEM_FLAG_ACTIVE) ||
         /* Item is owned. */
         ITEM_OWNER_NONE != items[i].owner ||
         /* Item is on a different TILEMAP. */
         0 != memory_strncmp_ptr(
            items[i].map_name, map->name,
            memory_strnlen_ptr(
               items[i].map_name, TILEMAP_NAME_MAX ) ) ||
         /* Item tile is not dirty. */
         /* !tilemap_is_dirty( items[i].y, items[i].x, map ) || */
         /* Item is off-screen. */
         items[i].x < gstate->screen_scroll_tx ||
            items[i].y < gstate->screen_scroll_ty ||
            items[i].x >= gstate->screen_scroll_tx + SCREEN_TW ||
            items[i].y >= gstate->screen_scroll_ty + SCREEN_TH
      ) {
         continue;
      }

      item_px = SCREEN_MAP_X + ((items[i].x * TILE_W) -
            gstate->screen_scroll_x);
      item_py = SCREEN_MAP_Y + ((items[i].y * TILE_H) -
            gstate->screen_scroll_y);

      graphics_blit_sprite_at(
         items[i].sprite_id, 0, 0,
         item_px, item_py, SPRITE_W, SPRITE_H );

   }

cleanup:

   if( NULL != map ) {
      map = (struct TILEMAP*)memory_unlock( state->map_handle );
   }
   
   if( NULL != items ) {
      items = (struct ITEM*)memory_unlock( state->items_handle );
   }
}

void topdown_draw( struct DSEKAI_STATE* state ) {
   struct TOPDOWN_STATE* gstate = NULL;
   int8_t i = 0;

   topdown_draw_tilemap( state );

   gstate = (struct TOPDOWN_STATE*)memory_lock( state->engine_state_handle );

   /* Draw crops. */
   topdown_draw_crops( state, gstate );

   /* Draw items without owners. */
   topdown_draw_items( state, gstate );

   /* Draw mobiles. */
   for( i = 0 ; DSEKAI_MOBILES_MAX > i ; i++ ) {
      topdown_draw_mobile( state, gstate, &(state->mobiles[i]) );
   }
   topdown_draw_mobile( state, gstate, &(state->player) );

   gstate = (struct TOPDOWN_STATE*)memory_unlock( state->engine_state_handle );
}

void topdown_focus_player( struct DSEKAI_STATE* state ) {
   struct TOPDOWN_STATE* gstate = NULL;
   int16_t player_x_px = 0,
      player_y_px = 0;

   gstate = (struct TOPDOWN_STATE*)memory_lock( state->engine_state_handle );

   player_x_px = state->player.coords.x * TILE_W;
   player_y_px = state->player.coords.y * TILE_H;

   gstate->screen_scroll_x_tgt = (player_x_px / SCREEN_MAP_W) * SCREEN_MAP_W;
   gstate->screen_scroll_x = (player_x_px / SCREEN_MAP_W) * SCREEN_MAP_W;
   gstate->screen_scroll_tx = gstate->screen_scroll_x / TILE_W;

   gstate->screen_scroll_y_tgt = (player_y_px / SCREEN_MAP_H) * SCREEN_MAP_H;
   gstate->screen_scroll_y = (player_y_px / SCREEN_MAP_H) * SCREEN_MAP_H;
   gstate->screen_scroll_ty = gstate->screen_scroll_y / TILE_H;

   debug_printf( 2, "player x, y: %d, %d (%d, %d)", 
      state->player.coords.x,
      state->player.coords.y,
      player_x_px, player_y_px );

   gstate = (struct TOPDOWN_STATE*)memory_unlock( state->engine_state_handle );
}

void topdown_animate( struct DSEKAI_STATE* state ) {
   struct TOPDOWN_STATE* gstate = NULL;
   struct TILEMAP* map = NULL;

   gstate = (struct TOPDOWN_STATE*)memory_lock( state->engine_state_handle );
   if( NULL == gstate ) {
      error_printf( "could not lock gstate" );
      goto cleanup;
   }

   map = (struct TILEMAP*)memory_lock( state->map_handle );
   if( NULL == map ) {
      error_printf( "could not lock tilemap" );
      goto cleanup;
   }

   /* Scroll the screen by one if the player goes off-screen. */
   if(
      state->player.coords.x >=
         gstate->screen_scroll_tx + SCREEN_TW
   ) {
      gstate->screen_scroll_x_tgt = gstate->screen_scroll_x + SCREEN_MAP_W;
      debug_printf( 2, "scrolling screen right to %d, %d...",
         gstate->screen_scroll_x_tgt, gstate->screen_scroll_y_tgt );

   } else if(
      state->player.coords.y >=
         gstate->screen_scroll_ty + SCREEN_TH
   ) {
      gstate->screen_scroll_y_tgt = gstate->screen_scroll_y + SCREEN_MAP_H;
      debug_printf( 2, "scrolling screen down to %d, %d...",
         gstate->screen_scroll_x_tgt, gstate->screen_scroll_y_tgt );

   } else if(
      state->player.coords.x < gstate->screen_scroll_tx
   ) {
      gstate->screen_scroll_x_tgt = gstate->screen_scroll_x - SCREEN_MAP_W;
      debug_printf( 2, "scrolling screen left to %d, %d...",
         gstate->screen_scroll_x_tgt, gstate->screen_scroll_y_tgt );

   } else if(
      state->player.coords.y < gstate->screen_scroll_ty
   ) {
      gstate->screen_scroll_y_tgt = gstate->screen_scroll_y - SCREEN_MAP_H;
      debug_printf( 2, "scrolling screen up to %d, %d...",
         gstate->screen_scroll_x_tgt, gstate->screen_scroll_y_tgt );
   }

   /*
   If the screen is scrolling, prioritize that before accepting more
   commands or drawing mobiles.
   */
   if(
      gstate->screen_scroll_x_tgt != gstate->screen_scroll_x ||
      gstate->screen_scroll_y_tgt != gstate->screen_scroll_y 
   ) {

      state->flags |= DSEKAI_FLAG_INPUT_BLOCKED;

      /* Increment map scroll. */
      if( gstate->screen_scroll_x_tgt > gstate->screen_scroll_x ) {
         gstate->screen_scroll_x += TILE_W;
         gstate->screen_scroll_tx = gstate->screen_scroll_x / TILE_W;
      }
      if( gstate->screen_scroll_y_tgt > gstate->screen_scroll_y ) {
         gstate->screen_scroll_y += TILE_H;
         gstate->screen_scroll_ty = gstate->screen_scroll_y / TILE_H;
      }
      if( gstate->screen_scroll_x_tgt < gstate->screen_scroll_x ) {
         gstate->screen_scroll_x -= TILE_W;
         gstate->screen_scroll_tx = gstate->screen_scroll_x / TILE_W;
      }
      if( gstate->screen_scroll_y_tgt < gstate->screen_scroll_y ) {
         gstate->screen_scroll_y -= TILE_H;
         gstate->screen_scroll_ty = gstate->screen_scroll_y / TILE_H;
      }

      tilemap_refresh_tiles( map );

      /* Check if map scroll finished. */
      if(
         gstate->screen_scroll_y == gstate->screen_scroll_y_tgt &&
         gstate->screen_scroll_x == gstate->screen_scroll_x_tgt
      ) {
         /* Screen scroll finished. */
         state->player.coords_prev.x =
            state->player.coords.x;
         state->player.coords_prev.y =
            state->player.coords.y;
         state->player.steps_x = 0;
         state->player.steps_y = 0;
      }
   } else {
      state->flags &= ~DSEKAI_FLAG_INPUT_BLOCKED;
   }

   /* Refresh the tilemap if a transition is in progress. */
   if( 0 < (state->transition & DSEKAI_TRANSITION_MASK_FRAME) ) {
      tilemap_refresh_tiles( map );
   }

cleanup:

   if( NULL != gstate ) {
      assert( NULL != state );
      gstate = (struct TOPDOWN_STATE*)memory_unlock(
         state->engine_state_handle );
   }

   if( NULL != map ) {
      map = (struct TILEMAP*)memory_unlock( state->map_handle );
   }
}

int16_t topdown_setup( struct DSEKAI_STATE* state ) {
   int16_t retval = 1;
   struct TILEMAP* map = NULL;

   assert( (MEMORY_HANDLE)NULL == state->engine_state_handle );
   state->engine_state_handle = memory_alloc(
         sizeof( struct TOPDOWN_STATE ), 1 );
   if( (MEMORY_HANDLE)NULL == state->engine_state_handle ) {
      error_printf( "unable to allocate engine state!" );
      retval = DSEKAI_ERROR_ALLOCATE;
      goto cleanup;
   }

   /* Make sure the tilemap is drawn at least once behind any initial windows.
    */
   topdown_focus_player( state );
   map = (struct TILEMAP*)memory_lock( state->map_handle );
   if( NULL != map ) {
      tilemap_refresh_tiles( map );
      map = (struct TILEMAP*)memory_unlock( state->map_handle );
   }
   topdown_draw_tilemap( state );

   /* Show status window. */
   window_push(
      WINDOW_ID_STATUS, 0, WINDOW_TYPE_WINDOW, 0,
      0, SCREEN_MAP_Y + SCREEN_MAP_H, STATUS_WINDOW_W, STATUS_WINDOW_H,
      GRAPHICS_COLOR_WHITE, GRAPHICS_COLOR_BLACK, 0,
      0, NULL );

   /* Force reset the weather to start the animation. */
   map = (struct TILEMAP*)memory_lock( state->map_handle );
   if( NULL != map ) {
      tilemap_set_weather( map, (TILEMAP_FLAG_WEATHER_MASK & map->flags) );
      map = (struct TILEMAP*)memory_unlock( state->map_handle );
   }

   engines_set_transition(
      state, DSEKAI_TRANSITION_TYPE_CURTAIN, DSEKAI_TRANSITION_DIR_OPEN );

   state->engine_state = ENGINE_STATE_RUNNING;

cleanup:
   
   return retval;
}

void topdown_shutdown( struct DSEKAI_STATE* state ) {
   debug_printf( 3, "shutting down topdown engine..." );
   window_pop( WINDOW_ID_STATUS );
}

int16_t topdown_input( char in_char, struct DSEKAI_STATE* state ) {
   int16_t retval = 1;
   struct CROP_PLOT* plot = NULL;
   struct TILEMAP* map = NULL;

   map = (struct TILEMAP*)memory_lock( state->map_handle );
   if( NULL == map ) {
      goto cleanup;
   }

   switch( in_char ) {
   case INPUT_KEY_UP:
#ifndef NO_ENGINE_EDITOR
      if( EDITOR_FLAG_ACTIVE == (EDITOR_FLAG_ACTIVE & state->editor.flags) ) {
         state->editor.coords.y--;
      } else {
#endif /* !NO_ENGINE_EDITOR */
         engines_handle_movement( MOBILE_DIR_NORTH, state, map );
#ifndef NO_ENGINE_EDITOR
      }
#endif /* !NO_ENGINE_EDITOR */
      break;

   case INPUT_KEY_LEFT:
#ifndef NO_ENGINE_EDITOR
      if( EDITOR_FLAG_ACTIVE == (EDITOR_FLAG_ACTIVE & state->editor.flags) ) {
         state->editor.coords.x--;
      } else {
#endif /* !NO_ENGINE_EDITOR */
         engines_handle_movement( MOBILE_DIR_WEST, state, map );
#ifndef NO_ENGINE_EDITOR
      }
#endif /* !NO_ENGINE_EDITOR */
      break;

   case INPUT_KEY_DOWN:
#ifndef NO_ENGINE_EDITOR
      if( EDITOR_FLAG_ACTIVE == (EDITOR_FLAG_ACTIVE & state->editor.flags) ) {
         state->editor.coords.y++;
      } else {
#endif /* !NO_ENGINE_EDITOR */
         engines_handle_movement( MOBILE_DIR_SOUTH, state, map );
#ifndef NO_ENGINE_EDITOR
      }
#endif /* !NO_ENGINE_EDITOR */
      break;

   case INPUT_KEY_RIGHT:
#ifndef NO_ENGINE_EDITOR
      if( EDITOR_FLAG_ACTIVE == (EDITOR_FLAG_ACTIVE & state->editor.flags) ) {
         state->editor.coords.x++;
      } else {
#endif /* !NO_ENGINE_EDITOR */
         engines_handle_movement( MOBILE_DIR_EAST, state, map );
#ifndef NO_ENGINE_EDITOR
      }
#endif /* !NO_ENGINE_EDITOR */
      break;

   case INPUT_KEY_OK:
#ifndef NO_ENGINE_EDITOR
      if( EDITOR_FLAG_ACTIVE == (EDITOR_FLAG_ACTIVE & state->editor.flags) ) {
         tilemap_advance_tile_id( map,
            state->editor.coords.x,
            state->editor.coords.y );
         state->editor.flags |= EDITOR_FLAG_FORCE_FRAME;
      } else if( NULL != (plot = crop_find_plot(
         map,
         state->player.coords.x +
            gc_mobile_x_offsets[mobile_get_dir( &(state->player) )],
         state->player.coords.y +
            gc_mobile_y_offsets[mobile_get_dir( &(state->player) )],
         state
      )) ) {
         /* Try to harvest facing crop plot. */

         if( 0 == plot->crop_gid ) {
            window_prefab_system_dialog(
               "There is no crop\ngrowing here!",
               WINDOW_PREFAB_DEFAULT_FG(), WINDOW_PREFAB_DEFAULT_BG() );
         } else if( CROP_STAGE_MAX > (CROP_FLAG_STAGE_MASK & plot->flags) ) {
            window_prefab_system_dialog(
               "This crop is\nnot ready!",
               WINDOW_PREFAB_DEFAULT_FG(), WINDOW_PREFAB_DEFAULT_BG() );
         } else {
            crop_harvest( ITEM_OWNER_PLAYER, plot, state, map );
         }

      } else {
#endif /* !NO_ENGINE_EDITOR */
         /* Try to pickup items laying on the ground. */
         /* TODO: Differentiate pickup and interact. */
         item_pickup_xy(
            state->player.coords.x, state->player.coords.y,
            ITEM_OWNER_PLAYER, map, state );

         /* Try to interact with facing mobile. */
         mobile_interact(
            &(state->player),
            mobile_get_facing( &(state->player), state ),
            map );
#ifndef NO_ENGINE_EDITOR
      }
#endif /* !NO_ENGINE_EDITOR */
      break;
   }

cleanup:

   if( NULL != map ) {
      map = (struct TILEMAP*)memory_unlock( state->map_handle );
   }

   return retval;
}

