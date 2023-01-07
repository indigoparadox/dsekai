
#define CROPS_C
#include "dsekai.h"

void crop_grow( struct CROP_PLOT* plot ) {
   uint8_t crop_stage = (plot->flags & CROP_FLAG_STAGE_MASK);

   /* TODO: Implement a rolling timer that counts down and compensates for
    *       rollover.
    */
   if( plot->next_at_ticks <= graphics_get_ms() ) {
      /* TODO: Growth modifiers based on terrain, powerups, etc. */
      crop_stage++;
      plot->flags &= ~CROP_FLAG_STAGE_MASK;
      plot->flags |= crop_stage;
      plot->next_at_ticks = graphics_get_ms() + plot->cycle;
      debug_printf( 1, "crop gid: %d grew to stage: %d, next stage at: %d",
         plot->crop_gid, crop_stage, plot->next_at_ticks );
   }
}

void crop_grow_all( struct DSEKAI_STATE* state ) {
   int16_t i = 0;

   if( !engines_state_lock( state ) ) {
      goto cleanup;     
   }

   for( i = 0 ; state->crops_sz > i ; i++ ) {
      if(
         CROP_FLAG_ACTIVE != (CROP_FLAG_ACTIVE & state->crops[i].flags) ||
         0 == state->crops[i].crop_gid ||
         CROP_STAGE_MAX == (CROP_FLAG_STAGE_MASK & state->crops[i].flags)
      ) {
         continue;
      }

      crop_grow( &(state->crops[i]) );
   }

cleanup:
   return;
}

struct CROP_PLOT* crop_find_plot(
   uint8_t x, uint8_t y, struct DSEKAI_STATE* state
) {
   int8_t i = 0;
   struct CROP_PLOT* plot = NULL;

   if( !engines_state_lock( state ) ) {
      goto cleanup;     
   }

   /* Find a plot in front of the user. */
   for( i = 0 ; state->crops_sz > i ; i++ ) {
      if(
         CROP_FLAG_ACTIVE != (CROP_FLAG_ACTIVE & state->crops[i].flags) ||
         state->crops[i].map_gid != state->tilemap->gid ||
         x != state->crops[i].coords.x || y != state->crops[i].coords.y
      ) {
         continue;
      }

      plot = (struct CROP_PLOT*)&(state->crops[i]);
      break;
   }

cleanup:

   return plot;
}

int8_t crop_plant(
   uint8_t crop_gid, struct CROP_PLOT* plot, struct DSEKAI_STATE* state
) {
   int8_t retval = 1,
      i = 0;

   if( !engines_state_lock( state ) ) {
      goto cleanup;     
   }

   /* Find the crop definition. */
   i = crop_get_def_idx( crop_gid, state );
   if( 0 > i ) {
      retval = CROP_ERROR_DEF_NOT_FOUND;
      error_printf( "could not find crop def for: %d", crop_gid );
      goto cleanup;
   }

   /* TODO: Make sure another crop isn't already growing here! */

   /* Plant the crop. */
   plot->crop_gid = crop_gid;
   plot->next_at_ticks = graphics_get_ms() + state->tilemap->crop_defs[i].cycle;
   plot->cycle = state->tilemap->crop_defs[i].cycle;

   debug_printf( 1, "planted crop: %d at: %d, %d on map %d, next stage at: %d",
      plot->crop_gid, plot->coords.x, plot->coords.y,
      plot->map_gid, plot->next_at_ticks );

cleanup:

   return retval;
}

int8_t crop_harvest(
   int8_t harvester_id, struct CROP_PLOT* plot, struct DSEKAI_STATE* state
) {
   int8_t i = 0,
      retval = 1;

   if( !engines_state_lock( state ) ) {
      goto cleanup;     
   }

   assert( 0 < plot->crop_gid );
   assert( CROP_STAGE_MAX == (CROP_FLAG_STAGE_MASK & plot->flags) );

   /* Find the crop definition. */
   i = crop_get_def_idx( plot->crop_gid, state );
   if( 0 > i ) {
      retval = CROP_ERROR_DEF_NOT_FOUND;
      error_printf( "could not find crop def for: %d", plot->crop_gid );
      goto cleanup;
   }

   debug_printf( 1, "harvesting plot at: %d, %d on map: %d",
      plot->coords.x, plot->coords.y, plot->map_gid );

   /* Give the harvested crop to the harvester. */
   item_stack_or_add(
      state->tilemap->crop_defs[i].produce_gid, harvester_id, state );

   /* Update the plot status to reflect harvest. */
   plot->flags &= ~CROP_FLAG_STAGE_MASK;
   if(
      CROP_DEF_FLAG_REGROWS !=
      (state->tilemap->crop_defs[i].flags & CROP_DEF_FLAG_REGROWS)
   ) {
      plot->crop_gid = 0;
      plot->next_at_ticks = 0;
      plot->cycle = 0;
   } else {
      /* Crop does regrow. */
      plot->next_at_ticks = graphics_get_ms() + plot->cycle;
      plot->flags |= (CROP_FLAG_STAGE_MASK & 2);
   }

cleanup:

   return retval;
}

int8_t crop_get_def_idx( uint8_t gid, struct DSEKAI_STATE* state ) {
   int8_t i = 0,
      retval = -1;

   if( !engines_state_lock( state ) ) {
      goto cleanup;     
   }

   /* Check TILEMAP::crop_defs for ::CROP_DEF with same GID. */
   for( i = 0 ; TILEMAP_CROP_DEFS_MAX > i ; i++ ) {
      if( state->tilemap->crop_defs[i].gid == gid ) {
         retval = i;
         break;
      }
   }

   if( 0 > retval ) { 
      error_printf( "could not find crop def for: %d", gid );
   }

cleanup:
   return retval;

}

