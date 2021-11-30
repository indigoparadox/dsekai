
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

   switch( in_char ) {
   case INPUT_KEY_UP:
      engines_handle_movement( MOBILE_DIR_NORTH, state );
      break;

   case INPUT_KEY_LEFT:
      engines_handle_movement( MOBILE_DIR_WEST, state );
      break;

   case INPUT_KEY_DOWN:
      engines_handle_movement( MOBILE_DIR_SOUTH, state );
      break;

   case INPUT_KEY_RIGHT:
      engines_handle_movement( MOBILE_DIR_EAST, state );
      break;

   case INPUT_KEY_QUIT:
      window_pop( WINDOW_ID_STATUS, state );
      retval = 0;
   }

   return retval;
}

void pov_animate( struct DSEKAI_STATE* state ) {

}

void pov_draw( struct DSEKAI_STATE* state ) {
   double plane_x = 0,
      plane_y = 0.66,
      camera_x = 0,
      side_dist_x = 0,
      side_dist_y = 0,
      delta_dist_x = 0,
      delta_dist_y = 0,
      ray_dir_x = 0,
      ray_dir_y = 0,
      perp_wall_dist = 0;
   int16_t x = 0,
      ray_map_tx = 0,
      ray_map_ty = 0,
      wall_hit = 0,
      wall_side = 0,
      ray_step_x = 0,
      ray_step_y = 0,
      line_px_height = 0,
      line_px_start = 0,
      line_px_end = 0;
   int8_t tile_id = 0;
   GRAPHICS_COLOR color;

   graphics_draw_block(
      0, 0, SCREEN_MAP_W, SCREEN_MAP_H, GRAPHICS_COLOR_BLACK );

   for( x = 0 ; SCREEN_MAP_W > x ; x++ ) {
      /* Setup ray direction and position. */
      camera_x = 2 * x / SCREEN_MAP_W - 1;
      ray_dir_x = gc_mobile_x_offsets[state->player.dir] + plane_x * camera_x;
      ray_dir_y = gc_mobile_y_offsets[state->player.dir] + plane_y * camera_x;
      ray_map_tx = state->player.coords.x;
      ray_map_ty = state->player.coords.y;

      /* Set ray distance to next tile side. */
      delta_dist_x = (0 == ray_dir_x) ? 1e30 : abs( 1 / ray_dir_x );
      delta_dist_y = (0 == ray_dir_y) ? 1e30 : abs( 1 / ray_dir_y );

      /* Figure out the ray direction. */

      if( 0 > ray_dir_x ) {
         ray_step_x = -1;
         side_dist_x = (state->player.coords.x - ray_map_tx) * delta_dist_x;
      } else {
         ray_step_x = 1;
         side_dist_x =
            (state->player.coords.x + 1.0 - ray_map_tx) * delta_dist_x;
      }

      if( 0 > ray_dir_y ) {
         ray_step_y = -1;
         side_dist_y = (state->player.coords.x - ray_map_ty) * delta_dist_y;
      } else {
         ray_step_y = 1;
         side_dist_y =
            (state->player.coords.y + 1.0 - ray_map_ty) * delta_dist_y;
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
            wall_hit = 2;
         }

         if(
            /* TILEMAP_TILESET_FLAG_BLOCK ==
            (state->map.tileset[tile_id].flags & TILEMAP_TILESET_FLAG_BLOCK) */
            1 == tile_id
         ) {
            wall_hit = 1;
         }
      }

      /* Figure out the wall distance. */
      if( 0 == wall_side ) {
         perp_wall_dist = side_dist_x - delta_dist_x;
      } else {
         perp_wall_dist = side_dist_y - delta_dist_y;
      }

      line_px_height = (int16_t)(SCREEN_MAP_H / perp_wall_dist);
      line_px_start = -line_px_height / 2 + SCREEN_MAP_H / 2;
      if( 0 > line_px_start ) {
         line_px_start = 0;
      }
      line_px_end = line_px_height / 2 + SCREEN_MAP_H / 2;
      if( SCREEN_MAP_H < line_px_end ) {
         line_px_end = SCREEN_MAP_H - 1;
      }

      if( wall_side ) {
         color = GRAPHICS_COLOR_WHITE;
      } else {
         color = GRAPHICS_COLOR_CYAN;
      }

      /* debug_printf( 3, "%d %d %d %d",
         x, line_px_height, line_px_start, line_px_end ); */
         
      if( 1 == wall_hit ) {
         graphics_draw_line(
            SCREEN_MAP_X + x,
            SCREEN_MAP_Y + line_px_start,
            SCREEN_MAP_X + x,
            SCREEN_MAP_Y + line_px_end, 1, color );
      }
   }
}

