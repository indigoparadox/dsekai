
#define CROPS_C
#include "dsekai.h"

void crop_grow( struct CROP_PLOT* plot, struct DSEKAI_STATE* state ) {
   uint8_t crop_stage = (plot->flags & CROP_FLAG_STAGE_MASK);

   if( plot->next_at_ticks <= graphics_get_ms() ) {
      /* TODO: Growth modifiers, etc. */
      crop_stage++;
      plot->flags &= ~CROP_FLAG_STAGE_MASK;
      plot->flags |= crop_stage;
      plot->next_at_ticks = graphics_get_ms() + plot->cycle;
      debug_printf( 1, "crop gid: %d grew to stage: %d, next stage at: %d",
         plot->crop_gid, crop_stage, plot->next_at_ticks );

#if 0
      /* Don't bother checking the current tilemap; dirty is harmless. */
      t = (struct TILEMAP*)memory_lock( state->map_handle );
      if( NULL != t ) {
         tilemap_set_dirty( plot->coords.x, plot->coords.y, t );
         t = (struct TILEMAP*)memory_unlock( state->map_handle );
      }
#endif
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

      crop_grow( &(state->crops[i]), state );
   }
}

struct CROP_PLOT* crop_find_plot(
   struct TILEMAP* t, uint8_t x, uint8_t y, const struct DSEKAI_STATE* state
) {
   int8_t i = 0;
   struct CROP_PLOT* plot = NULL;

   /* Find a plot in front of the user. */
   for( i = 0 ; DSEKAI_CROPS_MAX > i ; i++ ) {
      if(
         CROP_FLAG_ACTIVE != (CROP_FLAG_ACTIVE & state->crops[i].flags) ||
         0 != memory_strncmp_ptr(
            state->crops[i].map_name, t->name, TILEMAP_NAME_MAX ) ||
         x != state->crops[i].coords.x || y != state->crops[i].coords.y
      ) {
         continue;
      }

      plot = (struct CROP_PLOT*)&(state->crops[i]);
      break;
   }

   return plot;
}

int8_t crop_plant(
   uint8_t crop_gid, struct CROP_PLOT* plot, struct TILEMAP* crop_def_map
) {
   int8_t retval = 1,
      i = 0;
   struct CROP_DEF* crop_def = NULL;

   /* Find the crop definition. */
   i = crop_get_def_idx( crop_gid, crop_def_map );
   if( 0 > i ) {
      retval = CROP_ERROR_DEF_NOT_FOUND;
      error_printf( "could not find crop def for: %d", crop_gid );
      goto cleanup;
   }
   crop_def = &(crop_def_map->crop_defs[i]);

   /* Plant the crop. */
   plot->crop_gid = crop_gid;
   plot->next_at_ticks = graphics_get_ms() + crop_def->cycle;
   plot->cycle = crop_def->cycle;

   debug_printf( 1, "planted crop: %d at: %d, %d on map %s, next stage at: %d",
      plot->crop_gid, plot->coords.x, plot->coords.y,
      plot->map_name, plot->next_at_ticks );

cleanup:

   return retval;
}

int8_t crop_harvest(
   int8_t harvester_id, struct CROP_PLOT* plot, struct DSEKAI_STATE* state,
   struct TILEMAP* crop_def_map
) {
   struct CROP_DEF* crop_def = NULL;
   int8_t i = 0,
      retval = 1;

   assert( 0 < plot->crop_gid );
   assert( CROP_STAGE_MAX == (CROP_FLAG_STAGE_MASK & plot->flags) );

   /* Find the crop definition. */
   i = crop_get_def_idx( plot->crop_gid, crop_def_map );
   if( 0 > i ) {
      retval = CROP_ERROR_DEF_NOT_FOUND;
      error_printf( "could not find crop def for: %d", plot->crop_gid );
      goto cleanup;
   }
   crop_def = &(crop_def_map->crop_defs[i]);

   debug_printf( 1, "harvesting plot at: %d, %d on map: %s",
      plot->coords.x, plot->coords.y, plot->map_name );

   /* Give the harvested crop to the harvester. */
   item_stack_or_add(
      crop_def->produce_gid, harvester_id, crop_def_map, state );

   /* Update the plot status to reflect harvest. */
   plot->flags &= ~CROP_FLAG_STAGE_MASK;
   if( CROP_DEF_FLAG_REGROWS != (crop_def->flags & CROP_DEF_FLAG_REGROWS) ) {
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

int8_t crop_get_def_idx( uint8_t gid, const struct TILEMAP* map ) {
   int8_t i = 0,
      retval = -1;

   /* Check TILEMAP::crop_defs for ::CROP_DEF with same GID. */
   for( i = 0 ; TILEMAP_CROP_DEFS_MAX > i ; i++ ) {
      if( map->crop_defs[i].gid == gid ) {
         retval = i;
         break;
      }
   }

   if( 0 > retval ) { 
      error_printf( "unable to find this crop def on the current tilemap" );
   }

   return retval;

}

