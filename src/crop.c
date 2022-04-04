
#define CROPS_C
#include "dsekai.h"

void crop_grow( struct DSEKAI_STATE* state, struct CROP_PLOT* plot ) {
   uint8_t crop_stage = (plot->flags & CROP_FLAG_STAGE_MASK);

   if( plot->next_at_ticks <= graphics_get_ms() ) {
      /* TODO: Growth modifiers, etc. */
      crop_stage++;
      plot->flags &= ~CROP_FLAG_STAGE_MASK;
      plot->flags |= crop_stage;
      plot->next_at_ticks = graphics_get_ms() + plot->cycle;
      debug_printf( 1, "crop %d grew to stage %d, next stage at: %d",
         plot->crop_gid, crop_stage, plot->next_at_ticks );
   }
}

void crop_grow_all( struct DSEKAI_STATE* state ) {
   int16_t i = 0;

   for( i = 0 ; DSEKAI_CROPS_MAX > i ; i++ ) {
      if(
         CROP_FLAG_ACTIVE != (CROP_FLAG_ACTIVE & state->crops[i].flags) ||
         0 == state->crops[i].crop_gid ||
         CROP_STAGE_MAX == (CROP_FLAG_STAGE_MASK & state->crops[i].flags)
      ) {
         continue;
      }

      crop_grow( state, &(state->crops[i]) );
   }
}

int8_t crop_plant(
   struct DSEKAI_STATE* state, uint8_t crop_gid, uint8_t x, uint8_t y
) {
   int8_t retval = 1,
      i = 0;
   struct CROP_PLOT* plot = NULL;
   struct CROP_DEF* crop_def = NULL;

   /* Find the crop definition. */
   i = crop_get_def_idx( state, crop_gid );
   if( 0 > i ) {
      retval = CROP_ERROR_DEF_NOT_FOUND;
      error_printf( "could not find crop def for %d", crop_gid );
      goto cleanup;
   }
   crop_def = &(state->map.crop_defs[i]);

   /* Find a plot in front of the user. */
   for( i = 0 ; DSEKAI_CROPS_MAX > i ; i++ ) {
      if(
         CROP_FLAG_ACTIVE != (CROP_FLAG_ACTIVE & state->crops[i].flags) ||
         0 != memory_strncmp_ptr(
            state->crops[i].map_name, state->map.name, TILEMAP_NAME_MAX ) ||
         x != state->crops[i].coords.x || y != state->crops[i].coords.y
      ) {
         continue;
      }

      plot = &(state->crops[i]);
      break;
   }

   if( NULL == plot ) {
      retval = CROP_ERROR_PLOT_NOT_FOUND;
      error_printf( "could not find plot" );
      goto cleanup;
   }

   /* Plant the crop. */
   plot->crop_gid = crop_gid;
   plot->next_at_ticks = graphics_get_ms() + crop_def->cycle;
   plot->cycle = crop_def->cycle;

   debug_printf( 1, "planted crop %d at %d, %d on map %s, next stage at: %d",
      plot->crop_gid, x, y, state->map.name, plot->next_at_ticks );

cleanup:

   return retval;
}

int8_t crop_get_def_idx( struct DSEKAI_STATE* state, uint8_t gid ) {
   int8_t i = 0,
      retval = -1;

   /* Check TILEMAP::crop_defs for ::CROP_DEF with same GID. */
   for( i = 0 ; TILEMAP_CROP_DEFS_MAX > i ; i++ ) {
      if( state->map.crop_defs[i].gid == gid ) {
         retval = i;
         break;
      }
   }

   if( 0 > retval ) { 
      error_printf( "unable to find this crop def on the current tilemap" );
   }

   return retval;

}

