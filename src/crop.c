
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

void crop_grow_all( struct CROP_PLOT* crops, int16_t crops_sz ) {
   int16_t i = 0;

   for( i = 0 ; DSEKAI_CROPS_MAX > i ; i++ ) {
      if(
         CROP_FLAG_ACTIVE != (CROP_FLAG_ACTIVE & crops[i].flags) ||
         0 == crops[i].crop_gid ||
         CROP_STAGE_MAX == (CROP_FLAG_STAGE_MASK & crops[i].flags)
      ) {
         continue;
      }

      crop_grow( &(crops[i]) );
   }
}

struct CROP_PLOT* crop_find_plot(
   struct TILEMAP* t, uint8_t x, uint8_t y,
   struct CROP_PLOT* crops, int16_t crops_sz,
   const struct DSEKAI_STATE* state
) {
   int8_t i = 0;
   struct CROP_PLOT* plot = NULL;

   /* Find a plot in front of the user. */
   for( i = 0 ; crops_sz > i ; i++ ) {
      if(
         CROP_FLAG_ACTIVE != (CROP_FLAG_ACTIVE & crops[i].flags) ||
         crops[i].map_gid != t->gid ||
         x != crops[i].coords.x || y != crops[i].coords.y
      ) {
         continue;
      }

      plot = (struct CROP_PLOT*)&(crops[i]);
      break;
   }

   return plot;
}

int8_t crop_plant(
   uint8_t crop_gid, struct CROP_PLOT* plot,
   const struct CROP_DEF* crop_defs, int16_t crop_defs_sz
) {
   int8_t retval = 1,
      i = 0;

   /* Find the crop definition. */
   i = crop_get_def_idx( crop_gid, crop_defs, crop_defs_sz );
   if( 0 > i ) {
      retval = CROP_ERROR_DEF_NOT_FOUND;
      error_printf( "could not find crop def for: %d", crop_gid );
      goto cleanup;
   }

   /* TODO: Make sure another crop isn't already growing here! */

   /* Plant the crop. */
   plot->crop_gid = crop_gid;
   plot->next_at_ticks = graphics_get_ms() + crop_defs[i].cycle;
   plot->cycle = crop_defs[i].cycle;

   debug_printf( 1, "planted crop: %d at: %d, %d on map %d, next stage at: %d",
      plot->crop_gid, plot->coords.x, plot->coords.y,
      plot->map_gid, plot->next_at_ticks );

cleanup:

   return retval;
}

int8_t crop_harvest(
   int8_t harvester_id, struct CROP_PLOT* plot,
   struct TILEMAP* crop_def_t,
   const struct CROP_DEF* crop_defs, int16_t crop_defs_sz,
   struct ITEM* items, int16_t items_sz,
   struct DSEKAI_STATE* state
) {
   int8_t i = 0,
      retval = 1;

   assert( 0 < plot->crop_gid );
   assert( CROP_STAGE_MAX == (CROP_FLAG_STAGE_MASK & plot->flags) );

   /* Find the crop definition. */
   i = crop_get_def_idx( plot->crop_gid, crop_defs, crop_defs_sz );
   if( 0 > i ) {
      retval = CROP_ERROR_DEF_NOT_FOUND;
      error_printf( "could not find crop def for: %d", plot->crop_gid );
      goto cleanup;
   }

   debug_printf( 1, "harvesting plot at: %d, %d on map: %d",
      plot->coords.x, plot->coords.y, plot->map_gid );

   /* Give the harvested crop to the harvester. */
   item_stack_or_add(
      crop_defs[i].produce_gid, harvester_id, crop_def_t, items, items_sz,
      state );

   /* Update the plot status to reflect harvest. */
   plot->flags &= ~CROP_FLAG_STAGE_MASK;
   if( CROP_DEF_FLAG_REGROWS != (crop_defs[i].flags & CROP_DEF_FLAG_REGROWS) ) {
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

int8_t crop_get_def_idx(
   uint8_t gid, const struct CROP_DEF* crop_defs, int16_t crop_defs_sz
) {
   int8_t i = 0,
      retval = -1;

   /* Check TILEMAP::crop_defs for ::CROP_DEF with same GID. */
   for( i = 0 ; crop_defs_sz > i ; i++ ) {
      if( crop_defs[i].gid == gid ) {
         retval = i;
         break;
      }
   }

   if( 0 > retval ) { 
      error_printf( "unable to find this crop def on the current tilemap" );
   }

   return retval;

}

