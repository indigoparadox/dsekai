
#include "dsekai.h"

int16_t pov_setup( struct DSEKAI_STATE* state ) {
   int16_t retval = 1;
   struct POV_STATE* gstate = NULL;

   assert( (MEMORY_HANDLE)NULL == state->engine_state_handle );
   state->engine_state_handle =
      memory_alloc( sizeof( struct POV_STATE ), 1 );

   gstate = (struct POV_STATE*)memory_lock( state->engine_state_handle );

   gstate->dirty = 1;

   gstate = (struct POV_STATE*)memory_unlock( state->engine_state_handle );

   state->engine_state = ENGINE_STATE_RUNNING;

   return retval;
}

void pov_shutdown( struct DSEKAI_STATE* state ) {

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
   int16_t step_x;
   int16_t step_y;
   double side_dist_x;
   double side_dist_y;
   double delta_dist_x;
   double delta_dist_y;
   double wall_x;
   double wall_dist;
   int8_t wall_side;
   int8_t tile_id;
};

static int8_t pov_cast_ray(
   double x_orig, double y_orig, int16_t x,
   struct POV_RAY* ray, struct TILEMAP* t,
   uint8_t* minimap
) {
   int16_t wall_hit = 0;

   while( !wall_hit ) {

      /* Increment the ray. */
      if( ray->side_dist_x < ray->side_dist_y ) {
         ray->side_dist_x += ray->delta_dist_x;
         ray->map_tx += ray->step_x;
         ray->wall_side = 0;
      } else {
         ray->side_dist_y += ray->delta_dist_y;
         ray->map_ty += ray->step_y;
         ray->wall_side = 1;
      }

      /* Detect if the ray went off the map. */
      if(
         ray->map_ty > TILEMAP_TH || 0 > ray->map_ty ||
         ray->map_tx > TILEMAP_TW || 0 > ray->map_tx
      ) {
         wall_hit = 2;
      }

      minimap[(ray->map_ty * TILEMAP_TW) + ray->map_tx] = 1;

      /* Determine the tile hit by the ray. */
      ray->tile_id = tilemap_get_tile_id( t, ray->map_tx, ray->map_ty );
      if( 0 > ray->tile_id ) {
         error_printf( "invalid tile_id at %d, %d", ray->map_tx, ray->map_ty );
         wall_hit = 2;
      }

      if(
         TILEMAP_TILESET_FLAG_BLOCK ==
         (t->tileset[ray->tile_id].flags & TILEMAP_TILESET_FLAG_BLOCK)
      ) {
         debug_printf( 3,
            "tile id at screen %d, %dx%d: %d (%d)", x,
               ray->map_tx, ray->map_ty, ray->tile_id, ray->wall_side );
         wall_hit = 1;
         minimap[(ray->map_ty * TILEMAP_TW) + ray->map_tx] = 2;
      }
   }

   if( 2 == wall_hit ) {
      return -1;
   }

   /* Figure out the wall distance. */
   if( 0 == ray->wall_side ) {
      ray->wall_dist = ray->side_dist_x - ray->delta_dist_x;
   } else {
      ray->wall_dist = ray->side_dist_y - ray->delta_dist_y;
   }

   /* Get the X coordinate on the wall for the texture. */
   if( 0 == ray->wall_side ) {
      ray->wall_x = x_orig + ray->wall_dist * ray->dir_y;
   } else {
      ray->wall_x = x_orig + ray->wall_dist * ray->dir_x;
   }
   ray->wall_x -= floor( ray->wall_x );

   return ray->tile_id;
}

void pov_draw_wall_x( int16_t x, struct POV_RAY* ray, struct TILEMAP* t ) {
   double tex_step = 0,
      tex_pos = 0;
   int16_t tex_x = 0,
      tex_y = 0,
      y = 0,
      line_px_height = 0,
      line_px_start = 0,
      line_px_end = 0;
         
   line_px_height = (int32_t)(SCREEN_MAP_H / ray->wall_dist);
   line_px_start = -line_px_height / 2 + SCREEN_MAP_H / 2;
   if( 0 > line_px_start ) {
      line_px_start = 0;
   }
   line_px_end = line_px_height / 2 + SCREEN_MAP_H / 2;
   if( SCREEN_MAP_H < line_px_end ) {
      line_px_end = SCREEN_MAP_H - 1;
   }

   /* Figure out the X coordinate on the texture bitmap. */
   tex_x = (int32_t)(ray->wall_x * (double)TILE_W);
   if( 0 == ray->wall_side && 0 < ray->dir_x ) {
      tex_x = TILE_W - tex_x - 1;
   }
   if( 1 == ray->wall_side && 0 > ray->dir_y ) {
      tex_x = TILE_W - tex_x - 1;
   }

   tex_step = 1.0 * TILE_H / line_px_height;
   tex_pos =
      (line_px_start - SCREEN_MAP_H / 2 + SCREEN_MAP_H / 2) * tex_step;

   for( y = line_px_start ; line_px_end > y ; y++ ) {
      tex_y = (int32_t)tex_pos & (TILE_H - 1);
      tex_pos += tex_step;
      if( !ray->wall_side || (ray->wall_side && 0 == y % 2) ) {
         graphics_blit_tile_at(
            t->tileset[ray->tile_id].image,
            tex_x, tex_y,
            x, y,
            1, 1 );
      }
   }
}

void pov_draw( struct DSEKAI_STATE* state ) {
   struct POV_STATE* gstate = NULL;
   double plane_x = 0,
      plane_y = 0.66,
      camera_x = 0;
   int16_t x = 0,
      y = 0;
   int8_t tile_id = 0;
   struct POV_RAY ray;
   GRAPHICS_COLOR color;

   gstate = (struct POV_STATE*)memory_lock( state->engine_state_handle );

   if( !(gstate->dirty) ) {
      goto cleanup;
   }
   
   memory_zero_ptr( gstate->minimap, TILEMAP_TH * TILEMAP_TW );

   graphics_draw_block(
      0, 0, SCREEN_MAP_W, SCREEN_MAP_H, GRAPHICS_COLOR_BLACK );

   debug_printf( 3, "casting..." );

   for( x = 0 ; SCREEN_MAP_W > x ; x++ ) {
      memory_zero_ptr( &ray, sizeof( struct POV_RAY ) );
      /* Setup ray direction and position. */
      camera_x = 2 * x / (double)SCREEN_MAP_W - 1;
      /*
      ray.dir_x = gc_mobile_x_offsets[state->player.dir] + plane_x * camera_x;
      ray.dir_y = gc_mobile_y_offsets[state->player.dir] + plane_y * camera_x;
      */
      ray.dir_x = -1.0 + plane_x * camera_x;
      ray.dir_y = 0.0 + plane_y * camera_x;
      ray.map_tx = state->player.coords.x;
      ray.map_ty = state->player.coords.y;

      /* Set ray distance to next tile side. */
      ray.delta_dist_x = (0 == ray.dir_x) ? 1e30 : fabs( 1 / ray.dir_x );
      ray.delta_dist_y = (0 == ray.dir_y) ? 1e30 : fabs( 1 / ray.dir_y );
      /*
      ray.delta_dist_x = 
         sqrt( 1 + (ray.dir_y * ray.dir_y) / (ray.dir_x * ray.dir_x) );
      ray.delta_dist_y =
         sqrt( 1 + (ray.dir_x * ray.dir_x) / (ray.dir_y * ray.dir_y) );
      */

      debug_printf( 3, "x %d %f", x, camera_x );

      /*assert(
         (x >= 80 && ray.delta_dist_x >= 0) ||
         (x < 80 && ray.delta_dist_x <= 0) );*/

      /* Figure out the ray direction. */

      if( 0 > ray.dir_x ) {
         ray.step_x = -1;
         ray.side_dist_x =
            (state->player.coords.x - ray.map_tx) * ray.delta_dist_x;
      } else {
         ray.step_x = 1;
         ray.side_dist_x =
            (ray.map_tx + 1 - (double)(state->player.coords.x)) * ray.delta_dist_x;
      }

      if( 0 > ray.dir_y ) {
         ray.step_y = -1;
         ray.side_dist_y =
            (state->player.coords.x - ray.map_ty) * ray.delta_dist_y;
      } else {
         ray.step_y = 1;
         ray.side_dist_y =
            (ray.map_ty + 1 - (double)(state->player.coords.y)) * ray.delta_dist_y;
      }

      /* Launch the ray! */
      tile_id = pov_cast_ray(
         state->player.coords.x,
         state->player.coords.y,
         x, &ray, &(state->map), gstate->minimap );
      if( 0 > tile_id ) {
         /* Ray went off the map. */
         continue;
      }

      pov_draw_wall_x( x, &ray, &(state->map) );
   }

   for( y = 0 ; TILEMAP_TH > y ; y++ ) {
      for( x = 0 ; TILEMAP_TW > x ; x++ ) {
         switch( gstate->minimap[(y * TILEMAP_TW) + x] ) {
         case 2:
            color = GRAPHICS_COLOR_CYAN;
            break;

         case 1:
            color = GRAPHICS_COLOR_WHITE;
            break;

         default:
            color = GRAPHICS_COLOR_BLACK;
            break;
         }
      
         graphics_draw_px( MINIMAP_X + x, MINIMAP_Y + y, color );
      }
   }

   graphics_draw_px(
      MINIMAP_X + state->player.coords.x,
      MINIMAP_Y + state->player.coords.y,
      GRAPHICS_COLOR_MAGENTA );

   gstate->dirty = 0;

cleanup:

   if( NULL != gstate ) {
      gstate = memory_unlock( state->engine_state_handle );
   }

}

