
#include "dsekai.h"

#include <math.h>

const double gc_pov_dir_x[4] = {
   /* SOUTH */
   0.0,
   /* NORTH */
   0.0,
   /* EAST */
   1.0,
   /* WEST */
   -1.0
};

const double gc_pov_dir_y[4] = {
   /* SOUTH */
   1.0,
   /* NORTH */
   -1.0,
   /* EAST */
   0.0,
   /* WEST */
   0.0
};

const double gc_pov_plane_x[4] = {
   /* SOUTH */
   -0.66,
   /* NORTH */
   0.66,
   /* EAST */
   0.0,
   /* WEST */
   0.0
};

const double gc_pov_plane_y[4] = {
   /* SOUTH */
   0.0,
   /* NORTH */
   0.0,
   /* EAST */
   0.66,
   /* WEST */
   -0.66
};

const uint8_t gc_pov_dir_turn_right[] = {
   /* SOUTH */
   MOBILE_DIR_WEST,
   /* NORTH */
   MOBILE_DIR_EAST,
   /* EAST */
   MOBILE_DIR_SOUTH,
   /* WEST */
   MOBILE_DIR_NORTH
};

const uint8_t gc_pov_dir_turn_left[] = {
   MOBILE_DIR_EAST,
   MOBILE_DIR_WEST,
   MOBILE_DIR_NORTH,
   MOBILE_DIR_SOUTH
};

const char gc_pov_compass[] = {
   'S', 'N', 'E', 'W'
};

int8_t g_pov_skybox_idx = 0;

int16_t pov_setup( struct DSEKAI_STATE* state ) {
   int16_t retval = 1;
   struct TILEMAP* t = NULL;
   /*
   struct POV_STATE* gstate = NULL;
   */

   assert( (MEMORY_HANDLE)NULL == state->engine_state_handle );
   state->engine_state_handle =
      memory_alloc( sizeof( struct POV_STATE ), 1 );

   /*
   gstate = (struct POV_STATE*)memory_lock( state->engine_state_handle );

   gstate = (struct POV_STATE*)memory_unlock( state->engine_state_handle );
   */

   /* Create the environmental animations. */
   /* TODO: Correct for initial facing direction? */
   g_pov_skybox_idx = animate_create(
      ANIMATE_TYPE_CLOUDS,
      ANIMATE_FLAG_WEATHER | ANIMATE_FLAG_BG | ANIMATE_FLAG_CLOUDS_ROTATE,
      0, 0, SCREEN_MAP_W, SCREEN_MAP_H / 2 );

   state->flags |= DSEKAI_FLAG_BLANK_FRAME;

   t = (struct TILEMAP*)memory_lock( state->map_handle );
   if( NULL != t ) {
      tilemap_refresh_tiles( t );
      t = (struct TILEMAP*)memory_unlock( state->map_handle );
   }

   engines_set_transition(
      state, DSEKAI_TRANSITION_TYPE_CURTAIN, DSEKAI_TRANSITION_DIR_OPEN );

   state->engine_state = ENGINE_STATE_RUNNING;

   return retval;
}

void pov_shutdown( struct DSEKAI_STATE* state ) {
   state->flags &= ~DSEKAI_FLAG_BLANK_FRAME;
}

int16_t pov_input(
   INPUT_VAL in_char, int16_t click_x, int16_t click_y,
   struct DSEKAI_STATE* state
) {
   int16_t retval = 1;
   uint8_t recreate_clouds = 0;
   struct TILEMAP* t = NULL;
   /*
   struct POV_STATE* gstate = NULL;

   gstate = (struct POV_STATE*)memory_lock( state->engine_state_handle );
   */

   t = (struct TILEMAP*)memory_lock( state->map_handle );
   if( NULL == t ) {
      error_printf( "could not lock tilemap" );
      goto cleanup;
   }

   if( g_input_key_up == in_char ) {
      /* Tank controls: move in the direction we're facing. */
      if(
         0 < engines_input_movement(
            &(state->player), mobile_get_dir( &(state->player) ), state )
      ) {
         mobile_walk_start( &(state->player),
            mobile_get_dir( &(state->player) ) );
         tilemap_refresh_tiles( t );
      }
      /* gstate->dirty = 1; */

   } else if( g_input_key_left == in_char ) {
      /* pathfind_test_dir( MOBILE_DIR_WEST, state, t ); */
      mobile_set_dir( &(state->player),
         gc_pov_dir_turn_left[mobile_get_dir( &(state->player) )] );
      tilemap_refresh_tiles( t );
      if(
         0 == mobile_get_dir( &(state->player) ) ||
         1 == mobile_get_dir( &(state->player) )
      ) {
         recreate_clouds = 1;
      } else {
         recreate_clouds = 2;
      }
      /* gstate->dirty = 1; */

   } else if( g_input_key_down == in_char ) {
      /* TODO: Should this be south, or the opposite-facing dir? */
      if(
         0 < engines_input_movement(
            &(state->player), MOBILE_DIR_SOUTH, state )
      ) {
         mobile_walk_start( &(state->player), MOBILE_DIR_SOUTH );
         tilemap_refresh_tiles( t );
      }
      /* gstate->dirty = 1; */

   } else if( g_input_key_right == in_char ) { 
      /* pathfind_test_dir( MOBILE_DIR_EAST, state, t ); */
      mobile_set_dir( &(state->player),
         gc_pov_dir_turn_right[mobile_get_dir( &(state->player) )] );
      tilemap_refresh_tiles( t );
      if(
         2 == mobile_get_dir( &(state->player) ) ||
         3 == mobile_get_dir( &(state->player) )
      ) {
         recreate_clouds = 1;
      } else {
         recreate_clouds = 2;
      }
      /* gstate->dirty = 1; */

   } else if( g_input_key_ok == in_char ) {
      /* Try to interact with facing mobile. */
      mobile_interact(
         &(state->player),
         mobile_get_facing( state->player.coords[1].x, state->player.coords[1].y,
            mobile_get_dir( &(state->player) ), state ), state );
#ifdef POV_DEBUG_INC
      gstate->inc++;
      debug_printf( 3, "inc: %d", gstate->inc );
#endif /* POV_DEBUG_INC */
   }
   
   if( 1 == recreate_clouds ) {
      animate_stop( g_pov_skybox_idx );
      g_pov_skybox_idx = animate_create(
         ANIMATE_TYPE_CLOUDS,
         ANIMATE_FLAG_WEATHER | ANIMATE_FLAG_BG,
         0, 0, SCREEN_MAP_W, SCREEN_MAP_H / 2 );
   } else if( 2 == recreate_clouds ) {
      animate_stop( g_pov_skybox_idx );
      g_pov_skybox_idx = animate_create(
         ANIMATE_TYPE_CLOUDS,
         ANIMATE_FLAG_WEATHER | ANIMATE_FLAG_BG | ANIMATE_FLAG_CLOUDS_ROTATE,
         0, 0, SCREEN_MAP_W, SCREEN_MAP_H / 2 );
   }

   /*
   gstate = (struct POV_STATE*)memory_unlock( state->engine_state_handle );
   */

cleanup:
   
   if( NULL != t ) {
      t = (struct TILEMAP*)memory_unlock( state->map_handle );
   }

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
   double camera_x;
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
         ray->map_ty >= TILEMAP_TH || 0 > ray->map_ty ||
         ray->map_tx >= TILEMAP_TW || 0 > ray->map_tx
      ) {
         wall_hit = 2;
         continue;
      }

      minimap[(ray->map_ty * TILEMAP_TW) + ray->map_tx] = 1;

      /* Determine the tile hit by the ray. */
      ray->tile_id = tilemap_get_tile_id( t, ray->map_tx, ray->map_ty );
      if( 0 > ray->tile_id ) {
         error_printf( "invalid tile_id at %d, %d", ray->map_tx, ray->map_ty );
         wall_hit = 2;

      } else if(
         TILESET_FLAG_BLOCK ==
         (t->tileset[ray->tile_id].flags & TILESET_FLAG_BLOCK)
      ) {
#if 0
         if(
            TILEMAP_TILE_FLAG_DIRTY !=
            (TILEMAP_TILE_FLAG_DIRTY &
            t->tiles_flags[(ray->map_ty * TILEMAP_TW) + ray->map_tx])
         ) {
            /* Wall is not dirty. */
            wall_hit = 2;
         }

         /* Mark wall dirty. */
         t->tiles_flags[(ray->map_ty * TILEMAP_TW) + ray->map_tx] |=
            TILEMAP_TILE_FLAG_DIRTY;
#endif

         debug_printf( 0,
            "tile id at screen %d, %dx%d: %d (%d)", x,
               ray->map_tx, ray->map_ty, ray->tile_id, ray->wall_side );
         wall_hit = 1;

         /* Draw wall on minimap. */
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

void pov_draw_wall_x(
   int16_t x, struct POV_RAY* ray, struct TILEMAP* t, struct POV_STATE* gstate
) {
   double tex_step = 0,
      tex_pos = 0;
   int32_t tex_x = 0,
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

   tex_step = 2.0 * (2 * TILE_H) / (2 * line_px_height);
   tex_pos =
      (line_px_start - SCREEN_MAP_H / 2 + SCREEN_MAP_H / 2) * tex_step;

   if(
      (!ray->wall_side && 0 != ray->dir_x) ||
      (ray->wall_side && 0 != ray->dir_y)
   ) {
      /* We're drawing a wall orthoganal to the viewer. */
      tex_pos -= (2000
#ifdef POV_DEBUG_INC
         + gstate->inc
#endif /* POV_DEBUG_INC */
      ) / (line_px_height);
   }

   /* debug_printf( 3, "XXX" ); */
   for( y = line_px_start ; line_px_end > y ; y++ ) {
      /* debug_printf( 3, "y: %d tp: %f", y, tex_pos ); */
      tex_y = (int32_t)tex_pos & (TILE_H - 1);
      tex_pos += tex_step / 2;

      /* Create checkerboard effect on off sides. */
      if(
         !ray->wall_side && 
         ((0 == x % 2 && 1 == y % 2) || (1 == x % 2 && 0 == y % 2))
      ) {
         /* Draw black to blank out skybox. */
         graphics_draw_px( x, y, GRAPHICS_COLOR_BLACK );
         continue;
      }

      graphics_cache_blit_at(
         t->tileset[ray->tile_id].image_cache_id, GRAPHICS_INSTANCE_NONE,
         tex_x, tex_y,
         x, y,
         1, 1 );
   }
}

void pov_draw_minimap( uint8_t* minimap, struct MOBILE* player ) {
   int16_t x = 0,
      y = 0;
   GRAPHICS_COLOR color;

   /* Draw the minimap. */

   for( y = 0 ; TILEMAP_TH > y ; y++ ) {
      for( x = 0 ; TILEMAP_TW > x ; x++ ) {
         switch( minimap[(y * TILEMAP_TW) + x] ) {
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
      
         graphics_draw_px( MINIMAP_X + 1 + x, MINIMAP_Y + 1 + y, color );
      }
   }

   graphics_draw_rect(
      MINIMAP_X, MINIMAP_Y, TILEMAP_TW + 1, TILEMAP_TH + 1,
      1, GRAPHICS_COLOR_WHITE );

   graphics_draw_px(
      MINIMAP_X + 1 + player->coords[1].x,
      MINIMAP_Y + 1 + player->coords[1].y,
      GRAPHICS_COLOR_MAGENTA );

   graphics_char_at(
      gc_pov_compass[mobile_get_dir( player )], MINIMAP_X - 12, MINIMAP_Y,
      GRAPHICS_COLOR_WHITE, 0 );
}

void pov_draw( struct DSEKAI_STATE* state ) {
   int16_t x = 0;
   int8_t tile_id = 0;
   struct POV_RAY ray;
   struct POV_STATE* gstate = NULL;
   struct TILEMAP* t = NULL;

   gstate = (struct POV_STATE*)memory_lock( state->engine_state_handle );

   t = (struct TILEMAP*)memory_lock( state->map_handle );

   memory_zero_ptr( gstate->minimap, TILEMAP_TH * TILEMAP_TW );

   debug_printf( 0, "casting..." );

   for( x = 0 ; SCREEN_MAP_W > x ; x++ ) {
      memory_zero_ptr( &ray, sizeof( struct POV_RAY ) );
      /* Setup ray direction and position. */
      ray.camera_x = 2 * x / (double)SCREEN_MAP_W - 1;
      ray.dir_x = gc_pov_dir_x[mobile_get_dir( &(state->player) )] + 
         gc_pov_plane_x[mobile_get_dir( &(state->player) )] * ray.camera_x;
      ray.dir_y = gc_pov_dir_y[mobile_get_dir( &(state->player) )] +
         gc_pov_plane_y[mobile_get_dir( &(state->player) )] * ray.camera_x;
      ray.map_tx = state->player.coords[1].x;
      ray.map_ty = state->player.coords[1].y;

      /* Set ray distance to next tile side. */
      ray.delta_dist_x = (0 == ray.dir_x) ? 1e30 : fabs( 1 / ray.dir_x );
      ray.delta_dist_y = (0 == ray.dir_y) ? 1e30 : fabs( 1 / ray.dir_y );

      /* Figure out the ray direction. */

      if( 0 > ray.dir_x ) {
         ray.step_x = -1;
         ray.side_dist_x =
            ((double)(state->player.coords[1].x) - ray.map_tx) * ray.delta_dist_x;
      } else {
         ray.step_x = 1;
         ray.side_dist_x =
            (ray.map_tx + 1 - (double)(state->player.coords[1].x)) * ray.delta_dist_x;
      }

      if( 0 > ray.dir_y ) {
         ray.step_y = -1;
         ray.side_dist_y =
            ((double)(state->player.coords[1].y) - ray.map_ty) * ray.delta_dist_y;
      } else {
         ray.step_y = 1;
         ray.side_dist_y =
            (ray.map_ty + 1 - (double)(state->player.coords[1].y)) * ray.delta_dist_y;
      }

      /* Launch the ray! */
      tile_id = pov_cast_ray(
         state->player.coords[1].x,
         state->player.coords[1].y,
         x, &ray, t, gstate->minimap );
      if( 0 > tile_id ) {
         /* Ray went off the map. */
         continue;
      }

      pov_draw_wall_x( x, &ray, t, gstate );
   }

   if( NULL != t ) {
      t = (struct TILEMAP*)memory_unlock( state->map_handle );
   }

   if( NULL != gstate ) {
      /* Minimap needs valid gstate. */
      pov_draw_minimap( gstate->minimap, &(state->player) );

      gstate = memory_unlock( state->engine_state_handle );
   }

}

