
#include "dsekai.h"

int16_t pov_setup( struct DSEKAI_STATE* state ) {
   int16_t retval = 1;
   struct POV_STATE* gstate = NULL;

   assert( (MEMORY_HANDLE)NULL == state->engine_state_handle );
   state->engine_state_handle =
      memory_alloc( sizeof( struct POV_STATE ), 1 );

   gstate = (struct POV_STATE*)memory_lock( state->engine_state_handle );

   gstate = (struct POV_STATE*)memory_unlock( state->engine_state_handle );

   state->engine_state = ENGINE_STATE_RUNNING;

   return retval;
}

int16_t pov_input( char in_char, struct DSEKAI_STATE* state ) {
   int16_t retval = 1;
   struct POV_STATE* gstate = NULL;

   gstate = (struct POV_STATE*)memory_lock( state->engine_state_handle );

   switch( in_char ) {
   case INPUT_KEY_UP:
      engines_handle_movement( MOBILE_DIR_NORTH, state );
      gstate->dirty = 1;
      break;

   case INPUT_KEY_LEFT:
      engines_handle_movement( MOBILE_DIR_WEST, state );
      gstate->dirty = 1;
      break;

   case INPUT_KEY_DOWN:
      engines_handle_movement( MOBILE_DIR_SOUTH, state );
      gstate->dirty = 1;
      break;

   case INPUT_KEY_RIGHT:
      engines_handle_movement( MOBILE_DIR_EAST, state );
      gstate->dirty = 1;
      break;

   case INPUT_KEY_QUIT:
      window_pop( WINDOW_ID_STATUS, state );
      retval = 0;
   }

   gstate = (struct POV_STATE*)memory_unlock( state->engine_state_handle );

   return retval;
}

void pov_animate( struct DSEKAI_STATE* state ) {

}

struct POV_RAY {
   double dir_x;
   double dir_y;
   int16_t map_tx;
   int16_t map_ty;
};

void pov_cast_ray( int16_t x, struct POV_RAY* ray ) {

}

void pov_draw( struct DSEKAI_STATE* state ) {
   struct POV_STATE* gstate = NULL;
   double plane_x = 0,
      plane_y = 0.66,
      camera_x = 0,
      side_dist_x = 0,
      side_dist_y = 0,
      delta_dist_x = 0,
      delta_dist_y = 0,
      ray_dir_x = 0,
      ray_dir_y = 0,
      perp_wall_dist = 0,
      wall_x = 0,
      tex_step = 0,
      tex_pos = 0;
   int32_t x = 0,
      y = 0,
      ray_map_tx = 0,
      ray_map_ty = 0,
      wall_hit = 0,
      wall_side = 0,
      ray_step_x = 0,
      ray_step_y = 0,
      line_px_height = 0,
      line_px_start = 0,
      line_px_end = 0,
      tex_x = 0,
      tex_y = 0;
   int8_t tile_id = 0;
   GRAPHICS_COLOR color;

   gstate = (struct POV_STATE*)memory_lock( state->engine_state_handle );

   if( !(gstate->dirty) ) {
      goto cleanup;
   }

   graphics_draw_block(
      0, 0, SCREEN_MAP_W, SCREEN_MAP_H, GRAPHICS_COLOR_BLACK );

   for( x = 0 ; SCREEN_MAP_W > x ; x++ ) {
      /* Setup ray direction and position. */
      camera_x = 2 * x / (double)SCREEN_MAP_W - 1;
      /*ray_dir_x = gc_mobile_x_offsets[state->player.dir] + plane_x * camera_x;
      ray_dir_y = gc_mobile_y_offsets[state->player.dir] + plane_y * camera_x; */
      ray_dir_x = -1 + plane_x * camera_x;
      ray_dir_y = 0 + plane_y * camera_x;
      ray_map_tx = state->player.coords.x;
      ray_map_ty = state->player.coords.y;

      /* Set ray distance to next tile side. */
      delta_dist_x = (0 == ray_dir_x) ? 1e30 : fabs( 1 / ray_dir_x );
      delta_dist_y = (0 == ray_dir_y) ? 1e30 : fabs( 1 / ray_dir_y );
      /*
      delta_dist_x = 
         sqrt( 1 + (ray_dir_y * ray_dir_y) / (ray_dir_x * ray_dir_x) );
      delta_dist_y =
         sqrt( 1 + (ray_dir_x * ray_dir_x) / (ray_dir_y * ray_dir_y) );
      */

      /* Figure out the ray direction. */

      if( 0 > ray_dir_x ) {
         ray_step_x = -1;
         side_dist_x = (state->player.coords.x - ray_map_tx) * delta_dist_x;
      } else {
         ray_step_x = 1;
         side_dist_x =
            (ray_map_tx + 1 - (double)(state->player.coords.x)) * delta_dist_x;
      }

      if( 0 > ray_dir_y ) {
         ray_step_y = -1;
         side_dist_y = (state->player.coords.x - ray_map_ty) * delta_dist_y;
      } else {
         ray_step_y = 1;
         side_dist_y =
            (ray_map_ty + 1 - (double)(state->player.coords.y)) * delta_dist_y;
      }

      /* Launch the ray! */
      wall_hit = 0;
      while( !wall_hit ) {
         if( side_dist_x < side_dist_y ) {
            side_dist_x += delta_dist_x;
            ray_map_tx += ray_step_x;
            wall_side = 0;
         } else {
            side_dist_y += delta_dist_y;
            ray_map_ty += ray_step_y;
            wall_side = 1;
         }

         if(
            ray_map_ty > TILEMAP_TH || 0 > ray_map_ty ||
            ray_map_tx > TILEMAP_TW || 0 > ray_map_tx
         ) {
            wall_hit = 2;
         }

         tile_id = tilemap_get_tile_id( &(state->map), ray_map_tx, ray_map_ty );
         if( 0 > tile_id ) {
            error_printf( "invalid tile_id at %d, %d", ray_map_tx, ray_map_ty );
            wall_hit = 2;
         }

         if(
            TILEMAP_TILESET_FLAG_BLOCK ==
            (state->map.tileset[tile_id].flags & TILEMAP_TILESET_FLAG_BLOCK)
         ) {
            debug_printf( 3,
               "tile id at screen %d (%f), %dx%d: %d (%d)", x, camera_x,
                  ray_map_tx, ray_map_ty, tile_id, wall_side );
            wall_hit = 1;
         }
      }

      if( 2 == wall_hit ) {
         /* Ray went off the map. */
         continue;
      }

      /* Figure out the wall distance. */
      if( 0 == wall_side ) {
         perp_wall_dist = side_dist_x - delta_dist_x;
      } else {
         perp_wall_dist = side_dist_y - delta_dist_y;
      }

      line_px_height = (int32_t)(SCREEN_MAP_H / perp_wall_dist);
      line_px_start = -line_px_height / 2 + SCREEN_MAP_H / 2;
      if( 0 > line_px_start ) {
         line_px_start = 0;
      }
      line_px_end = line_px_height / 2 + SCREEN_MAP_H / 2;
      if( SCREEN_MAP_H < line_px_end ) {
         line_px_end = SCREEN_MAP_H - 1;
      }

      /* debug_printf( 3, "%d %d %d %d",
         x, line_px_height, line_px_start, line_px_end ); */

#if 0
      if( wall_side ) {
         color = GRAPHICS_COLOR_WHITE;
      } else {
         color = GRAPHICS_COLOR_CYAN;
      }

      /* debug_printf( 3, "%d %d %d %d",
         x, line_px_height, line_px_start, line_px_end ); */
         
      graphics_draw_line(
         SCREEN_MAP_X + x,
         SCREEN_MAP_Y + line_px_start,
         SCREEN_MAP_X + x,
         SCREEN_MAP_Y + line_px_end, 1, color );
#endif
         
      /* Get the X coordinate on the wall for the texture. */
      if( 0 == wall_side ) {
         wall_x = state->player.coords.x + perp_wall_dist * ray_dir_y;
      } else {
         wall_x = state->player.coords.x + perp_wall_dist * ray_dir_x;
      }
      wall_x -= floor( wall_x );

      /* Figure out the X coordinate on the texture bitmap. */
      tex_x = (int32_t)(wall_x * (double)TILE_W);
      if( 0 == wall_side && 0 < ray_dir_x ) {
         tex_x = TILE_W - tex_x - 1;
      }
      if( 1 == wall_side && 0 > ray_dir_y ) {
         tex_x = TILE_W - tex_x - 1;
      }

      tex_step = 1.0 * TILE_H / line_px_height;
      tex_pos =
         (line_px_start - SCREEN_MAP_H / 2 + SCREEN_MAP_H / 2) * tex_step;

      for( y = line_px_start ; line_px_end > y ; y++ ) {
         tex_y = (int32_t)tex_pos & (TILE_H - 1);
         tex_pos += tex_step;
         if( !wall_side || (wall_side && 0 == y % 2) ) {
            graphics_blit_tile_at(
               state->map.tileset[tile_id].image,
               tex_x, tex_y,
               x, y,
               1, 1 );
         }
      }
   }

   gstate->dirty = 0;

cleanup:

   if( NULL != gstate ) {
      gstate = memory_unlock( state->engine_state_handle );
   }

}

