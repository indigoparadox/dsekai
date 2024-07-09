
#include "dsekai.h"

#define PROFILE_FIELDS( f ) f( draw_mobiles ) f( draw_tilemap ) f( draw_items ) f( animate )
#include "profiler.h"

/**
 * \brief Draw currently on-screen portion of a TILEMAP.
 * \param state ::MEMORY_PTR to the current engine state, used to determine
 *              what is currently on-screen.
 */
static
void topdown_draw_tilemap(
   struct DSEKAI_STATE* state, struct TOPDOWN_STATE* gstate
) {
   int16_t x = 0,
      y = 0,
      i = 0;
   uint8_t tile_id = 0;
   uint16_t
      tile_px = 0, /* Tile on-screen X in pixels. */
      tile_py = 0, /* Tile on-screen Y in pixels. */
      viewport_tx2 = 0,
      viewport_ty2 = 0;

   profiler_set();

   if( !engines_state_lock( state ) ) {
      goto cleanup;     
   }

   /* We don't need to lock this since it's called from inside the engine's
    * graphics_lock loop.
    */

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
            tilemap_set_dirty( x, y, state->tilemap );
            
            /* If we're not forcing the frame, black out the block.
             * If we ARE, then just draw as usual after marking dirty above. */
            if(
               EDITOR_FLAG_FORCE_FRAME !=
               (EDITOR_FLAG_FORCE_FRAME & state->editor.flags)
            ) {
               /* Black out the block. */
               graphics_clear_block(
                  SCREEN_MAP_X + tile_px, SCREEN_MAP_Y + tile_py,
                  TILE_W, TILE_H );
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
               tilemap_set_dirty( x, y, state->tilemap );
            }
         }
#endif /* !NO_ANIMATE */

#ifndef NO_DIRTY_TILES
         if( !tilemap_is_dirty( x, y, state->tilemap ) ) {
            continue;
         }
#endif /* !NO_DIRTY_TILES */

         /* Sanity checks. */
         assert( y < TILEMAP_TH );
         assert( x < TILEMAP_TW );
         assert( y >= 0 );
         assert( x >= 0 );

         tilemap_unset_dirty( x, y, state->tilemap );

         /* Grab the left byte if even or the right if odd. */
         tile_id = tilemap_get_tile_id( state->tilemap, x, y );

         if( tile_id >= TILEMAP_TILESETS_MAX ) {
            error_printf( "invalid tile id: %d", tile_id );
            continue;
         }

         /* debug_printf( 3, "redraw tile %d, %d", tile_px, tile_py ); */
         
         /* Blit the tile. */
         graphics_cache_blit_at(
            tilemap_tile_get_image( &(state->tilemap->tileset[tile_id]) ),
            GRAPHICS_INSTANCE_TILEMAP,
            0, 0,
            SCREEN_MAP_X + tile_px, SCREEN_MAP_Y + tile_py,
            TILE_W, TILE_H );
      }
   }

cleanup:

   profiler_incr( draw_tilemap );
}

static void topdown_draw_crops(
   struct DSEKAI_STATE* state, struct TOPDOWN_STATE* gstate
) {
   int16_t i = 0,
      crop_idx = 0;
   struct CROP_PLOT* plot = NULL;
   int16_t plot_gfx;
   uint16_t plot_px = 0,
      plot_py = 0;
   uint8_t crop_stage = 0;
   struct CROP_DEF* crop_def = NULL;
   RESOURCE_ID plot_res_id;

   if( !engines_state_lock( state ) ) {
      goto cleanup;     
   }

   /* TODO: Use tile dirty bit to avoid redraw unless crop sprite changes. */

   /* Draw crops/plots. */
   for( i = 0 ; state->crops_sz > i ; i++ ) {
      plot = &(state->crops[i]);
      if(
         /* Crop plot is inactive. */
         CROP_FLAG_ACTIVE != (plot->flags & CROP_FLAG_ACTIVE) ||
         /* Crop is on a different TILEMAP. */
         plot->map_gid != state->tilemap->gid ||
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

      resource_id_from_name( &plot_res_id, CROP_STATIC_SPRITE_PLOT,
         RESOURCE_EXT_GRAPHICS );
      plot_gfx = graphics_cache_load_bitmap(
         plot_res_id, GRAPHICS_BMP_FLAG_TYPE_TILE );

      assert( 0 < plot_gfx );

      graphics_cache_blit_at(
         plot_gfx, DSEKAI_MOBILES_ONSCREEN + i,
         0, 0, plot_px, plot_py, TILE_W, TILE_H );

      /* Skip drawing crop if none planted or it hasn't germinated. */
      if(
         0 == plot->crop_gid ||
         0 > (crop_idx = crop_get_def_idx( plot->crop_gid, state ))
      ) {
         continue;
      }

      /* Make sure crop spritesheet is loaded. */
      /* TODO: Handle curses/ascii. */
      crop_def = &(state->tilemap->crop_defs[crop_idx]);
      if( 0 > crop_def->sprite_cache_id ) {
         crop_def->sprite_cache_id = retrogxc_load_bitmap(
            crop_def->sprite_name, 0 );
         maug_cleanup_if_lt( crop_def->sprite_cache_id, 0, "%d", -1 );
      }

      crop_stage = (plot->flags & CROP_FLAG_STAGE_MASK);

      if( 0 < crop_stage ) {
         /* Crop has germinated. */
         retrogxc_blit_at(
            crop_def->sprite_cache_id, NULL,
            (crop_stage - 1) * TILE_W, 0,
            plot_px, plot_py, TILE_W, TILE_H );

      } else {
         /* Crop is still seeds. */
         resource_id_from_name( &plot_res_id, CROP_STATIC_SPRITE_SEED,
            RESOURCE_EXT_GRAPHICS );
         plot_gfx = graphics_cache_load_bitmap(
            plot_res_id, GRAPHICS_BMP_FLAG_TYPE_SPRITE );
         graphics_cache_blit_at(
            /* For the instance, crops come after mobiles. */
            plot_gfx, DSEKAI_MOBILES_ONSCREEN + i,
            0, 0, plot_px, plot_py, TILE_W, TILE_H );
      }
   }

cleanup:
   return;
}

static void topdown_draw_mobile(
   struct DSEKAI_STATE* state, struct TOPDOWN_STATE* gstate,
   int16_t* p_onscreen_mobs, struct MOBILE* m
) {

   profiler_set();

   if( !engines_state_lock( state ) ) {
      goto cleanup;     
   }

   if(
      /* Don't draw inactive mobiles. */
      !mobile_is_active( m ) ||
      /* Don't draw mobiles from other tilemaps. */
      (MOBILE_MAP_GID_ALL != m->map_gid && m->map_gid != state->tilemap->gid) ||
      /* Pre-death blinking effect (skip negative even frames). */
      (0 > m->mp_hp && 0 == m->mp_hp % 2)
   ) {
      goto cleanup;
   }

   if(
      m->coords[1].x < gstate->screen_scroll_tx ||
      m->coords[1].y < gstate->screen_scroll_ty ||
      m->coords[1].x >= gstate->screen_scroll_tx + SCREEN_TW ||
      m->coords[1].y >= gstate->screen_scroll_ty + SCREEN_TH
   ) {
      /* Mobile is off-screen. */
      m->screen_px = -1;
      m->screen_py = -1;
      goto cleanup;
   }

   m->screen_px = 
      SCREEN_MAP_X + (m->coords[1].x * SPRITE_W) - gstate->screen_scroll_x;
   m->screen_py =
      SCREEN_MAP_Y + (m->coords[1].y * SPRITE_H) - gstate->screen_scroll_y;

   /* Figure out direction to offset steps in. */
   if( m->coords[0].x > m->coords[1].x ) {
      m->screen_px += SPRITE_W - m->steps_remaining;
   
   } else if( m->coords[0].x < m->coords[1].x ) {
      m->screen_px -= SPRITE_W - m->steps_remaining;

   } else if( m->coords[0].y > m->coords[1].y ) {
      m->screen_py += SPRITE_H - m->steps_remaining;
   
   } else if( m->coords[0].y < m->coords[1].y ) {
      m->screen_py -= SPRITE_H - m->steps_remaining;
   }

   /* Blit the mobile's current sprite/frame. */
   graphics_cache_blit_at(
      mobile_get_sprite( m ),
      *p_onscreen_mobs, state->ani_sprite_x, mobile_get_dir( m ) * SPRITE_H,
      m->screen_px, m->screen_py, SPRITE_W, SPRITE_H );

   /* Mobile is on-screen. */
   (*p_onscreen_mobs)++;

cleanup:
   profiler_incr( draw_mobiles );
}

void topdown_draw_items(
   struct DSEKAI_STATE* state, struct TOPDOWN_STATE* gstate
) {
   int16_t i = 0;
   uint16_t item_px = 0,
      item_py = 0;

   profiler_set();

   if( !engines_state_lock( state ) ) {
      goto cleanup;     
   }

   for( i = 0 ; state->items_sz > i ; i++ ) {
      if(
         /* Item is inactive. */
         !(state->items[i].flags & ITEM_FLAG_ACTIVE) ||
         /* Item is owned. */
         MOBILE_GID_NONE != state->items[i].owner ||
         /* Item is on a different TILEMAP. */
         state->items[i].map_gid != state->tilemap->gid ||
         /* Item tile is not dirty. */
         /* !tilemap_is_dirty( items[i].y, items[i].x, map ) || */
         /* Item is off-screen. */
         state->items[i].x < gstate->screen_scroll_tx ||
            state->items[i].y < gstate->screen_scroll_ty ||
            state->items[i].x >= gstate->screen_scroll_tx + SCREEN_TW ||
            state->items[i].y >= gstate->screen_scroll_ty + SCREEN_TH
      ) {
         continue;
      }

      item_px = SCREEN_MAP_X + ((state->items[i].x * TILE_W) -
            gstate->screen_scroll_x);
      item_py = SCREEN_MAP_Y + ((state->items[i].y * TILE_H) -
            gstate->screen_scroll_y);

      graphics_cache_blit_at(
         /* TODO: Handle curses/ascii. */
         state->items[i].sprite_cache_id,
         /* For the instance, items come after mobiles and crops. */
         DSEKAI_MOBILES_ONSCREEN + DSEKAI_CROPS_ONSCREEN + i,
         0, 0, item_px, item_py, SPRITE_W, SPRITE_H );

   }

cleanup:

   profiler_incr( draw_items );
}

void topdown_draw( struct DSEKAI_STATE* state ) {
   struct TOPDOWN_STATE* gstate = NULL;
   int16_t i = 0,
      onscreen_mobs = 0;

   maug_mlock( state->engine_state_handle, gstate );

   if( !engines_state_lock( state ) ) {
      goto cleanup;     
   }

   topdown_draw_tilemap( state, gstate );

   /* Draw crops. */
   topdown_draw_crops( state, gstate );

   /* Draw items without owners. */
   topdown_draw_items( state, gstate );

   /* Draw mobiles. */
   for( i = 0 ; state->mobiles_sz > i ; i++ ) {
      topdown_draw_mobile(
         state, gstate, &onscreen_mobs, &(state->mobiles[i]) );
   }
   /* TODO: Use a macro constant for the player sprite ID. */
   topdown_draw_mobile( state, gstate, &onscreen_mobs, &(state->player) );

cleanup:

   if( NULL != gstate ) {
      maug_munlock( state->engine_state_handle, gstate );
   }
}

static void topdown_focus_player(
   struct DSEKAI_STATE* state, struct TOPDOWN_STATE* gstate
) {
   int16_t player_x_px = 0,
      player_y_px = 0;

   player_x_px = state->player.coords[1].x * TILE_W;
   player_y_px = state->player.coords[1].y * TILE_H;

   gstate->screen_scroll_x_tgt = (player_x_px / SCREEN_MAP_W) * SCREEN_MAP_W;
   gstate->screen_scroll_x = (player_x_px / SCREEN_MAP_W) * SCREEN_MAP_W;
   gstate->screen_scroll_tx = gstate->screen_scroll_x / TILE_W;

   gstate->screen_scroll_y_tgt = (player_y_px / SCREEN_MAP_H) * SCREEN_MAP_H;
   gstate->screen_scroll_y = (player_y_px / SCREEN_MAP_H) * SCREEN_MAP_H;
   gstate->screen_scroll_ty = gstate->screen_scroll_y / TILE_H;

   debug_printf( 2, "player x, y: %d, %d (%d, %d)", 
      state->player.coords[1].x,
      state->player.coords[1].y,
      player_x_px, player_y_px );
}

void topdown_animate( struct DSEKAI_STATE* state ) {
   struct TOPDOWN_STATE* gstate = NULL;

   profiler_set();

   if( !engines_state_lock( state ) ) {
      goto cleanup;     
   }

   maug_mlock( state->engine_state_handle, gstate );
   if( NULL == gstate ) {
      error_printf( "could not lock gstate" );
      goto cleanup;
   }

   /* Scroll the screen by one if the player goes off-screen. */
   if(
      state->player.coords[1].x >=
         gstate->screen_scroll_tx + SCREEN_TW
   ) {
      gstate->screen_scroll_x_tgt = gstate->screen_scroll_x + SCREEN_MAP_W;
      debug_printf( 2, "scrolling screen right to %d, %d...",
         gstate->screen_scroll_x_tgt, gstate->screen_scroll_y_tgt );

   } else if(
      state->player.coords[1].y >=
         gstate->screen_scroll_ty + SCREEN_TH
   ) {
      gstate->screen_scroll_y_tgt = gstate->screen_scroll_y + SCREEN_MAP_H;
      debug_printf( 2, "scrolling screen down to %d, %d...",
         gstate->screen_scroll_x_tgt, gstate->screen_scroll_y_tgt );

   } else if(
      state->player.coords[1].x < gstate->screen_scroll_tx
   ) {
      gstate->screen_scroll_x_tgt = gstate->screen_scroll_x - SCREEN_MAP_W;
      debug_printf( 2, "scrolling screen left to %d, %d...",
         gstate->screen_scroll_x_tgt, gstate->screen_scroll_y_tgt );

   } else if(
      state->player.coords[1].y < gstate->screen_scroll_ty
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

      tilemap_refresh_tiles( state->tilemap );

      /* Check if map scroll finished. */
      if(
         gstate->screen_scroll_y == gstate->screen_scroll_y_tgt &&
         gstate->screen_scroll_x == gstate->screen_scroll_x_tgt
      ) {
         /* Screen scroll finished. */
         state->player.coords[0].x =
            state->player.coords[1].x;
         state->player.coords[0].y =
            state->player.coords[1].y;
         state->player.steps_remaining = 0;
      }
   } else {
      state->flags &= ~DSEKAI_FLAG_INPUT_BLOCKED;
   }

   /* Refresh the tilemap if a transition is in progress. */
   if( 0 < (state->transition & DSEKAI_TRANSITION_MASK_FRAME) ) {
      tilemap_refresh_tiles( state->tilemap );
   }

cleanup:

   if( NULL != gstate ) {
      assert( NULL != state );
      maug_munlock( state->engine_state_handle, gstate );
   }

   profiler_incr( animate );
}

int16_t topdown_setup( struct DSEKAI_STATE* state ) {
   int16_t retval = 1;
   struct TOPDOWN_STATE* gstate = NULL;

   debug_printf( 1, "setting up topdown engine..." );

   profiler_init();

   /* Allocate engine-specific state. */
   assert( (MAUG_MHANDLE)NULL == state->engine_state_handle );
   state->engine_state_handle = memory_alloc(
         sizeof( struct TOPDOWN_STATE ), 1 );
   if( (MAUG_MHANDLE)NULL == state->engine_state_handle ) {
      error_printf( "unable to allocate engine state!" );
      retval = DSEKAI_ERROR_ALLOCATE;
      goto cleanup;
   }

   if( !engines_state_lock( state ) ) {
      goto cleanup;     
   }

   maug_mlock( state->engine_state_handle, gstate );
   assert( NULL != gstate );

   /* Make sure the tilemap is drawn at least once behind any initial windows.
    */
   topdown_focus_player( state, gstate );
   tilemap_refresh_tiles( state->tilemap );
   topdown_draw_tilemap( state, gstate );

   /* Show status window. */
   window_push(
      WINDOW_ID_STATUS, 0, WINDOW_TYPE_WINDOW, 0,
      0, SCREEN_MAP_Y + SCREEN_MAP_H, STATUS_WINDOW_W, STATUS_WINDOW_H,
      GRAPHICS_COLOR_WHITE, GRAPHICS_COLOR_BLACK, 0,
      0, NULL );

   /* Force reset the weather to start the animation. */
   tilemap_set_weather(
      (TILEMAP_FLAG_WEATHER_MASK & state->tilemap->flags), state->tilemap );

   engines_set_transition(
      state, DSEKAI_TRANSITION_TYPE_CURTAIN, DSEKAI_TRANSITION_DIR_OPEN );

   state->engine_state = ENGINE_STATE_RUNNING;

   debug_printf( 1, "topdown engine setup complete!" );

cleanup:

   if( NULL != gstate ) {
      maug_munlock( state->engine_state_handle, gstate );
   }
   
   return retval;
}

void topdown_shutdown( struct DSEKAI_STATE* state ) {
   debug_printf( 3, "shutting down topdown engine..." );
   window_pop( WINDOW_ID_STATUS );
   profiler_print( "TOPDOWN" );
}

int16_t topdown_input(
   INPUT_VAL in_char, int16_t click_x, int16_t click_y,
   struct DSEKAI_STATE* state
) {
   int16_t retval = 1;
   int16_t pathfind_dir = 0;
   int8_t pathfind_x = 0;
   int8_t pathfind_y = 0;
   struct CROP_PLOT* plot = NULL;
   struct TOPDOWN_STATE* gstate = NULL;

   if( !engines_state_lock( state ) ) {
      goto cleanup;     
   }

   maug_mlock( state->engine_state_handle, gstate );
   if( NULL == gstate ) {
      goto cleanup;
   }

   if( g_input_key_up == in_char ) {
#ifndef NO_ENGINE_EDITOR
      if( EDITOR_FLAG_ACTIVE == (EDITOR_FLAG_ACTIVE & state->editor.flags) ) {
         state->editor.coords.y--;
      } else {
#endif /* !NO_ENGINE_EDITOR */
         if(
            0 <= engines_input_movement(
               &(state->player), MOBILE_DIR_NORTH, state )
         ) {
            mobile_walk_start( &(state->player), MOBILE_DIR_NORTH );
         }
#ifndef NO_ENGINE_EDITOR
      }
#endif /* !NO_ENGINE_EDITOR */

   } else if( g_input_key_left == in_char ) {
#ifndef NO_ENGINE_EDITOR
      if( EDITOR_FLAG_ACTIVE == (EDITOR_FLAG_ACTIVE & state->editor.flags) ) {
         state->editor.coords.x--;
      } else {
#endif /* !NO_ENGINE_EDITOR */
         if(
            0 <= engines_input_movement(
               &(state->player), MOBILE_DIR_WEST, state )
         ) {
            mobile_walk_start( &(state->player), MOBILE_DIR_WEST );
         }
#ifndef NO_ENGINE_EDITOR
      }
#endif /* !NO_ENGINE_EDITOR */

   } else if( g_input_key_down == in_char ) {
#ifndef NO_ENGINE_EDITOR
      if( EDITOR_FLAG_ACTIVE == (EDITOR_FLAG_ACTIVE & state->editor.flags) ) {
         state->editor.coords.y++;
      } else {
#endif /* !NO_ENGINE_EDITOR */
         if(
            0 <= engines_input_movement(
               &(state->player), MOBILE_DIR_SOUTH, state )
         ) {
            mobile_walk_start( &(state->player), MOBILE_DIR_SOUTH );
         }
#ifndef NO_ENGINE_EDITOR
      }
#endif /* !NO_ENGINE_EDITOR */

   } else if( g_input_key_right == in_char ) {
#ifndef NO_ENGINE_EDITOR
      if( EDITOR_FLAG_ACTIVE == (EDITOR_FLAG_ACTIVE & state->editor.flags) ) {
         state->editor.coords.x++;
      } else {
#endif /* !NO_ENGINE_EDITOR */
         if(
            0 <= engines_input_movement(
               &(state->player), MOBILE_DIR_EAST, state )
         ) {
            mobile_walk_start( &(state->player), MOBILE_DIR_EAST );
         }
#ifndef NO_ENGINE_EDITOR
      }
#endif /* !NO_ENGINE_EDITOR */

   } else if( INPUT_CLICK == in_char ) {
      debug_printf( 0, "trying to move to %d, %d...", click_x, click_y );

      /* Determine click tile coords. */
      pathfind_x = (gstate->screen_scroll_x + click_x - SCREEN_MAP_X) / TILE_W;
      pathfind_y = (gstate->screen_scroll_y + click_y - SCREEN_MAP_Y) / TILE_H;

      /* TODO: If click tile is adjacent, interact. */

      /* Move towards click. */
      pathfind_dir = pathfind_start(
         &(state->player), pathfind_x, pathfind_y, 3, 0, state );
      if( 0 > pathfind_dir ) {
         debug_printf( 1, "player pathfinding blocked" );
      } else {
         mobile_walk_start( &(state->player), pathfind_dir );
      }

   } else if( g_input_key_ok == in_char ) {
#ifndef NO_ENGINE_EDITOR
      if( EDITOR_FLAG_ACTIVE == (EDITOR_FLAG_ACTIVE & state->editor.flags) ) {
         tilemap_advance_tile_id( state->tilemap,
            state->editor.coords.x,
            state->editor.coords.y );
         state->editor.flags |= EDITOR_FLAG_FORCE_FRAME;
      } else if( NULL != (plot = crop_find_plot(
         state->player.coords[1].x +
            gc_mobile_x_offsets[mobile_get_dir( &(state->player) )],
         state->player.coords[1].y +
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
            crop_harvest( MOBILE_GID_PLAYER, plot, state );
         }

      } else {
#endif /* !NO_ENGINE_EDITOR */
         /* Try to pickup items laying on the ground. */
         /* TODO: Differentiate pickup and interact. */
         item_pickup_xy(
            state->player.coords[1].x, state->player.coords[1].y,
            MOBILE_GID_PLAYER, state );

         /* Try to interact with facing mobile. */
         mobile_interact(
            &(state->player),
            mobile_get_facing( state->player.coords[1].x, state->player.coords[1].y,
               mobile_get_dir( &(state->player) ), state ),
            state );
#ifndef NO_ENGINE_EDITOR
      }
#endif /* !NO_ENGINE_EDITOR */
   }

cleanup:

   if( NULL != gstate ) {
      maug_munlock( state->engine_state_handle, gstate );
   }

   return retval;
}

