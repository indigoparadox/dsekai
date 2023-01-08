
#define ITEM_C
#include "dsekai.h"

int8_t item_use_none(
   int16_t e_idx, MOBILE_GID owner_gid, struct DSEKAI_STATE* state
) {
   error_printf( "attempted to use invalid item" );
   return 0;
}

int8_t item_use_seed(
   int16_t e_idx, MOBILE_GID owner_gid, struct DSEKAI_STATE* state
) {
   int8_t retval = -1;
   uint8_t x = 0,
      y = 0,
      crop_gid = 0;
   struct CROP_PLOT* plot = NULL;
   struct MOBILE* user = NULL;

   if( !engines_state_lock( state ) ) {
      goto cleanup;     
   }

   /* Owner check. */
   user = mobile_from_gid( owner_gid, state );
   assert( NULL != user );

   crop_gid = state->items[e_idx].data;

   x = user->coords[1].x + gc_mobile_x_offsets[mobile_get_dir( user )];
   y = user->coords[1].y + gc_mobile_y_offsets[mobile_get_dir( user )];

   /* Get the plot in front of the user. */
   plot = crop_find_plot( x, y, state );
   if( NULL == plot ) {
#ifdef SCREEN_W
#ifndef NO_GUI
      window_prefab_system_dialog(
         "The soil is\ntoo firm!",
         WINDOW_PREFAB_DEFAULT_FG(), WINDOW_PREFAB_DEFAULT_BG() );
#endif /* !NO_GUI */
#endif /* SCREEN_W */
      error_printf( "could not find plot" );
      retval = ITEM_USED_FAILED;
      goto cleanup;
   }

   if( 0 < crop_plant( crop_gid, plot, state ) ) {
      /* Planting was successful. */
      item_decr_or_delete( e_idx, state );
   } else {
#ifdef SCREEN_W
#ifndef NO_GUI
      window_prefab_system_dialog(
         "This won't\ngrow here!",
         WINDOW_PREFAB_DEFAULT_FG(), WINDOW_PREFAB_DEFAULT_BG() );
#endif /* !NO_GUI */
#endif /* SCREEN_W */
      error_printf( "unable to plant seed" );
      retval = ITEM_USED_FAILED;
   }

cleanup:

   return retval;
}

int8_t item_use_food(
   int16_t e_idx, MOBILE_GID owner_gid, struct DSEKAI_STATE* state
) {
#if defined( DEPTH_VGA ) || defined( DEPTH_CGA )
#ifndef NO_ANIMATE
   int8_t anim_idx = 0;
#endif /* !NO_ANIMATE */
#endif /* DEPTH_VGA || DEPTH_CGA */
   char num_str[10];
   struct MOBILE* user = NULL;
   uint8_t food_val = 0;

   if( !engines_state_lock( state ) ) {
      goto cleanup;     
   }

   /* Owner check. */
   user = mobile_from_gid( owner_gid, state );
   assert( NULL != user );

   food_val = state->items[e_idx].data;

   item_decr_or_delete( e_idx, state );

   /* TODO: Different types of food for different stats? */
   debug_printf( 1, "mobile %d:%d mp/hp was: %d",
      user->map_gid, user->spawner_gid, user->mp_hp );
   mobile_incr_hp( user, food_val );
   debug_printf( 1, "mobile %d:%d mp/hp now: %d",
      user->map_gid, user->spawner_gid, user->mp_hp );

   dio_snprintf( num_str, 10, "+%d", food_val );

#ifndef NO_ANIMATE
   anim_idx = animate_create(
      ANIMATE_TYPE_STRING, ANIMATE_FLAG_FG, user->screen_px, user->screen_py,
      SPRITE_W, SPRITE_H );
   animate_set_string(
      anim_idx, num_str, 10,
#  ifdef DEPTH_VGA
      ANIMATE_COLOR_GREEN
#  else
      ANIMATE_COLOR_CYAN
#  endif /* DEPTH_VGA */
   );
#endif /* !NO_ANIMATE */

cleanup:

   return ITEM_USED_SUCCESSFUL;
}

int8_t item_use_shovel(
   int16_t e_idx, MOBILE_GID owner_gid, struct DSEKAI_STATE* state
) {
   /* TODO: Implement digging. */
   return ITEM_USED_FAILED;
}

int8_t item_use_editor(
   int16_t e_idx, MOBILE_GID owner_gid, struct DSEKAI_STATE* state
) {
#ifndef NO_ENGINE_EDITOR
   if( !engines_state_lock( state ) ) {
      goto cleanup;     
   }

   if(
      TILEMAP_FLAG_EDITABLE != (TILEMAP_FLAG_EDITABLE & state->tilemap->flags)
   ) {

      /* TODO: Display warning message on-screen. */
#ifdef SCREEN_W
#ifndef NO_GUI
      window_prefab_system_dialog(
         "This map cannot\nbe edited!",
         WINDOW_PREFAB_DEFAULT_FG(), WINDOW_PREFAB_DEFAULT_BG() );
#endif /* !NO_GUI */
#endif /* SCREEN_W */
      error_printf( "unable to edit current tilemap" );

   } else if(
      EDITOR_FLAG_ACTIVE == (EDITOR_FLAG_ACTIVE & state->editor.flags) ) {
      state->editor.flags &= ~EDITOR_FLAG_ACTIVE;
   } else {
      state->editor.flags |= EDITOR_FLAG_ACTIVE;
      state->editor.coords.x = state->player.coords[1].x;
      state->editor.coords.y = state->player.coords[1].y;
   }

cleanup:
#endif /* !NO_ENGINE_EDITOR */

   return -1;
}

int8_t item_use_material(
   int16_t e_idx, MOBILE_GID owner_gid, struct DSEKAI_STATE* state
) {
   /* TODO: Warn that materials cannot be used directly. */
   return 0;
}

int8_t item_use_watercan(
   int16_t e_idx, MOBILE_GID owner_gid, struct DSEKAI_STATE* state
) {
   /* TODO: Check for CROP_PLOTs that can be watered. */
   return 0;
}

int8_t item_use_hoe(
   int16_t e_idx, MOBILE_GID owner_gid, struct DSEKAI_STATE* state
) {
   int8_t retval = ITEM_USED_SUCCESSFUL;
   int16_t x = 0,
      y = 0,
      i = 0,
      crop_idx = -1;
   struct MOBILE* user = NULL;

   if( !engines_state_lock( state ) ) {
      goto cleanup;     
   }

   user = mobile_from_gid( owner_gid, state );
   assert( NULL != user );

   x = user->coords[1].x + gc_mobile_x_offsets[mobile_get_dir( user )];
   y = user->coords[1].y + gc_mobile_y_offsets[mobile_get_dir( user )];

   /* TODO: Move this to crop.c. */

   for( i = 0 ; state->crops_sz > i ; i++ ) {
      /* Find an empty CROP_PLOT. */
      if( CROP_FLAG_ACTIVE != (CROP_FLAG_ACTIVE & state->crops[i].flags) ) {
         if( 0 > crop_idx ) {
            /* First empty encountered, use this one later. */
            crop_idx = i;
         }
         continue;
      }

      /* Crop plot is not empty, but does it collide? */
      if(
         state->crops[i].map_gid == state->tilemap->gid &&
         x == state->crops[i].coords.x && y == state->crops[i].coords.y
      ) {
         retval = ITEM_USED_SUCCESSFUL;
         /* Destroy plot. */
         state->crops[i].flags &= ~CROP_FLAG_ACTIVE;
         crop_idx = -1;
         debug_printf( 1, "crop plot already exists; destroyed" );
         goto cleanup;
      }
   }

   if( 0 > crop_idx ) {
#ifdef SCREEN_W
#ifndef NO_GUI
      window_prefab_system_dialog(
         "Too many crops\nalready planted!",
         WINDOW_PREFAB_DEFAULT_FG(), WINDOW_PREFAB_DEFAULT_BG() );
#endif /* !NO_GUI */
#endif /* SCREEN_W */
      retval = ITEM_USED_FAILED;
      error_printf( "no available crop plots" );
      goto cleanup;
   }

   /* Create crop plot. */
   memory_zero_ptr(
      (MEMORY_PTR)&(state->crops[crop_idx]), sizeof( struct CROP_PLOT ) );
   state->crops[crop_idx].map_gid = state->tilemap->gid;
   state->crops[crop_idx].coords.x = x;
   state->crops[crop_idx].coords.y = y;
   state->crops[crop_idx].flags |= CROP_FLAG_ACTIVE;
   debug_printf( 2, "created crop plot %d at %d, %d on map %d",
      crop_idx,
      state->crops[crop_idx].coords.x, state->crops[crop_idx].coords.y,
      state->crops[crop_idx].map_gid );

cleanup:

   return retval;
}

int16_t item_exists_in_inventory(
   int16_t template_gid, MOBILE_GID owner_gid, struct DSEKAI_STATE* state
) {
   int16_t i = 0;

   if( !engines_state_lock( state ) ) {
      goto cleanup;     
   }

   /* Determine if the recipient has one of these already. */
   for( i = 0 ; state->items_sz > i ; i++ ) {
      /* item_break_if_last( items, i ); */
      if(
         ITEM_FLAG_ACTIVE == (state->items[i].flags & ITEM_FLAG_ACTIVE) &&
         state->items[i].gid == template_gid &&
         state->items[i].owner == owner_gid
      ) {
         debug_printf(
            2, "item gid %d exists in owner %d inventory at index: %d",
            template_gid, owner_gid, i );
         goto cleanup;
      }
   }

   debug_printf( 2, "item gid %d does not exist in owner %d inventory!",
      template_gid, owner_gid );
   i = ITEM_ERROR_NOT_FOUND;

cleanup:
 
   return i;
}

int16_t item_decr_or_delete( int16_t e_idx, struct DSEKAI_STATE* state ) {

   if( !engines_state_lock( state ) ) {
      goto cleanup;     
   }

   assert( ITEM_FLAG_ACTIVE == (ITEM_FLAG_ACTIVE & state->items[e_idx].flags) );

   if( 1 < item_get_count_flag( &(state->items[e_idx]) ) ) {
      /* Reduce the item's count. */
      item_incr_count( &(state->items[e_idx]), -1 );
   } else {
      /* Delete the item. */
      state->items[e_idx].flags &= ~ITEM_FLAG_ACTIVE;
      e_idx = ITEM_ERROR_NOT_FOUND;
   }

cleanup:

   return e_idx;
}

int16_t item_stack_or_add(
   int16_t template_gid, MOBILE_GID owner_gid, struct DSEKAI_STATE* state
) {
   int16_t e_idx = 0,
      i = 0;
   struct ITEM* e_def = NULL;

   if( !engines_state_lock( state ) ) {
      goto cleanup;     
   }

   /* Find the item definition with the given GID in tilemap item templates. */
   for( i = 0 ; state->items_sz > i ; i++ ) {
      item_break_if_last( state->tilemap->item_defs, i );
      if(
         state->tilemap->item_defs[i].gid == template_gid &&
         ITEM_FLAG_ACTIVE ==
            (ITEM_FLAG_ACTIVE & state->tilemap->item_defs[i].flags)
      ) {
         e_def = &(state->tilemap->item_defs[i]);
         debug_printf( 2, "found item with gid %d and owner %d at index: %d",
            template_gid, owner_gid, i );
         break;
      }
   }

   if( NULL == e_def ) {
      error_printf( "could not find item with gid: %d", template_gid );
      e_idx = ITEM_ERROR_MISSING_TEMPLATE;
      goto cleanup;
   }

   /* Determine if we're stacking or adding. */
   e_idx = item_exists_in_inventory( template_gid, owner_gid, state );

   if(
      /* Found a dupe (stacking). */
      0 <= e_idx &&
      /* Room on a stack. */
      gc_items_max[item_get_type_flag( e_def )] >=
         item_get_count_flag( &(state->items[e_idx]) ) + 1
   ) {
      debug_printf( 2, "adding item %d to mobile %d stack (%d)",
         template_gid, owner_gid,
         item_get_count_flag( &(state->items[e_idx]) ) );

   } else {
      /* Create item from template. */
      /* TODO: Check for full inventory vs ITEM_INVENTORY_MAX. */
      for( i = 0 ; state->items_sz > i ; i++ ) {
         /* Do NOT break on last item here, since we're just looking for a
          * free slot!
          */
         if( ITEM_FLAG_ACTIVE == (ITEM_FLAG_ACTIVE & state->items[i].flags) ) {
            /* Skip active items. */
            continue;
         }

         debug_printf(
            2, "creating item with gid %d and owner %d at index: %d",
            e_def->gid, owner_gid, i );

         memory_copy_ptr( 
            (MEMORY_PTR)&(state->items[i]), (CONST_MEMORY_PTR)e_def,
            sizeof( struct ITEM ) );

         state->items[i].owner = owner_gid;
         state->items[i].sprite_cache_id = -1;
         e_idx = i;

         /* Found and created the item, so quit. */
         break;
      }
   }

   /* Defs start with count of 0, or we found the e_idx above. */
   item_incr_count( &(state->items[e_idx]), 1 );

cleanup:

   if( 0 > e_idx ) {
      error_printf( "unable to give item!" );
   }

   return e_idx;
}

int16_t item_give_mobile(
   int16_t e_idx, MOBILE_GID owner_gid, struct DSEKAI_STATE* state
) {
   int16_t e_dest_idx = 0;
   int16_t e_gid = 0;

   if( !engines_state_lock( state ) ) {
      goto cleanup;     
   }

   /* Grab the source item's GID for use later. */
   assert( ITEM_FLAG_ACTIVE == (ITEM_FLAG_ACTIVE & state->items[e_idx].flags) );
   e_gid = state->items[e_idx].gid;
   
   debug_printf( 2, "giving item at index %d with gid %d to owner: %d",
      e_idx, e_gid, owner_gid );

   /* TODO: Check for full inventory vs ITEM_INVENTORY_MAX. */

   item_stack_or_add( e_gid, owner_gid, state );

   /* Since give was successful, remove former owner's copy. */
   item_decr_or_delete( e_idx, state );

cleanup:
   return e_dest_idx;
}

int8_t item_drop( int16_t e_idx, struct DSEKAI_STATE* state ) {
   struct ITEM* e = NULL;
   struct MOBILE* owner = NULL;

   if( !engines_state_lock( state ) ) {
      goto cleanup;     
   }

   assert( state->items_sz > e_idx );
   assert( 0 <= e_idx );
   e = &(state->items[e_idx]);
   assert( ITEM_FLAG_ACTIVE == (ITEM_FLAG_ACTIVE & e->flags) );

   owner = mobile_from_gid( e->owner, state );
   assert( NULL != owner );

   /* Set the item's new map coordinates based on previous owner. */
   e->x = owner->coords[1].x;
   e->y = owner->coords[1].y;
   e->owner = MOBILE_GID_NONE;
   e->map_gid = mobile_get_map_gid( owner, state );

   debug_printf( 2, "dropped item at %d, %d on tilemap %d",
      e->x, e->y, e->map_gid );

   /* TODO: Set tile as occupied. */
#if 0
   /* Set tile as dirty. */
   t = (struct TILEMAP*)memory_lock( state->map_handle );
   if( NULL != t ) {
      tilemap_set_dirty( item->x, item->y, t );
      t = (struct TILEMAP*)memory_unlock( state->map_handle );
   }
#endif

cleanup:

   return e_idx;
}

int16_t item_pickup_xy(
   uint8_t x, uint8_t y, MOBILE_GID owner_gid, struct DSEKAI_STATE* state
) {
   int16_t i = 0;

   if( !engines_state_lock( state ) ) {
      goto cleanup;     
   }

   for( i = 0 ; state->items_sz > i ; i++ ) {
      /* item_break_if_last( items, i ); */
      if(
         /* Skip inactive items. */
         !(ITEM_FLAG_ACTIVE & state->items[i].flags) ||
         /* Skip owned items. */
         MOBILE_GID_NONE != state->items[i].owner ||
         /* Skip items on other maps. */
         state->items[i].map_gid != state->tilemap->gid
      ) {
         continue;
      }

      if( x == state->items[i].x && y == state->items[i].y ) {
         debug_printf( 2, "assigned owner %d to item %d at %d, %d",
            owner_gid, i, x, y );
         /* TODO: Switch owner to use GID instead of index. */
         state->items[i].owner = owner_gid;

#if 0
         /* Set tile as dirty. */
         t = (struct TILEMAP*)memory_lock( state->map_handle );
         if( NULL != t ) {
            tilemap_set_dirty( plot->coords.x, plot->coords.y, t );
            t = (struct TILEMAP*)memory_unlock( state->map_handle );
         }
#endif
         break;
      }
   }

   /* Didn't pick anything up. */

cleanup:

   return i;
}

