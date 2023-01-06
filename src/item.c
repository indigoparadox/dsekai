
#define ITEM_C
#include "dsekai.h"

int8_t item_use_none(
   int16_t e_idx, int16_t owner_id,
   struct TILEMAP* t,
   struct ITEM* items, int16_t items_sz,
   struct MOBILE* mobiles, int16_t mobiles_sz,
   struct CROP_PLOT* crops, int16_t crops_sz,
   struct DSEKAI_STATE* state
) {
   error_printf( "attempted to use invalid item" );
   return 0;
}

int8_t item_use_seed(
   int16_t e_idx, int16_t owner_id,
   struct TILEMAP* t,
   struct ITEM* items, int16_t items_sz,
   struct MOBILE* mobiles, int16_t mobiles_sz,
   struct CROP_PLOT* crops, int16_t crops_sz,
   struct DSEKAI_STATE* state
) {
   int8_t retval = -1;
   uint8_t x = 0,
      y = 0,
      crop_gid = 0;
   struct CROP_PLOT* plot = NULL;
   struct MOBILE* user = NULL;

   /* Owner check. */
   if( 0 <= owner_id ) {
      user = &(mobiles[owner_id]);
   } else if( ITEM_OWNER_PLAYER == owner_id ) {
      user = &(state->player);
   }
   assert( NULL != user );

   crop_gid = items[e_idx].data;

   x = user->coords.x + gc_mobile_x_offsets[mobile_get_dir( user )];
   y = user->coords.y + gc_mobile_y_offsets[mobile_get_dir( user )];

   /* Get the plot in front of the user. */
   plot = crop_find_plot( t, x, y, crops, crops_sz, state );
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

   if( 0 < crop_plant( crop_gid, plot, t ) ) {
      /* Planting was successful. */
      item_decr_or_delete( e_idx, items, items_sz, state );
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
   int16_t e_idx, int16_t owner_id, 
   struct TILEMAP* t,
   struct ITEM* items, int16_t items_sz,
   struct MOBILE* mobiles, int16_t mobiles_sz,
   struct CROP_PLOT* crops, int16_t crops_sz,
   struct DSEKAI_STATE* state
) {
#if defined( DEPTH_VGA ) || defined( DEPTH_CGA )
#ifndef NO_ANIMATE
   int8_t anim_idx = 0;
#endif /* !NO_ANIMATE */
#endif /* DEPTH_VGA || DEPTH_CGA */
   char num_str[10];
   struct MOBILE* user = NULL;
   uint8_t food_val = 0;

   /* Owner check. */
   if( 0 <= owner_id ) {
      user = &(mobiles[owner_id]);
   } else if( ITEM_OWNER_PLAYER == owner_id ) {
      user = &(state->player);
   }
   assert( NULL != user );

   food_val = items[e_idx].data;

   item_decr_or_delete( e_idx, items, items_sz, state );

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

   return ITEM_USED_SUCCESSFUL;
}

int8_t item_use_shovel(
   int16_t e_idx, int16_t owner_id,
   struct TILEMAP* t,
   struct ITEM* items, int16_t items_sz,
   struct MOBILE* mobiles, int16_t mobiles_sz,
   struct CROP_PLOT* crops, int16_t crops_sz,
   struct DSEKAI_STATE* state
) {
   /* TODO: Implement digging. */
   return ITEM_USED_FAILED;
}

int8_t item_use_editor(
   int16_t e_idx, int16_t owner_id,
   struct TILEMAP* t,
   struct ITEM* items, int16_t items_sz,
   struct MOBILE* mobiles, int16_t mobiles_sz,
   struct CROP_PLOT* crops, int16_t crops_sz,
   struct DSEKAI_STATE* state
) {
#ifndef NO_ENGINE_EDITOR

   if( TILEMAP_FLAG_EDITABLE != (TILEMAP_FLAG_EDITABLE & t->flags) ) {

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
      state->editor.coords.x = state->player.coords.x;
      state->editor.coords.y = state->player.coords.y;
   }

#endif /* !NO_ENGINE_EDITOR */

   return -1;
}

int8_t item_use_material(
   int16_t e_idx, int16_t owner_id,
   struct TILEMAP* t,
   struct ITEM* items, int16_t items_sz,
   struct MOBILE* mobiles, int16_t mobiles_sz,
   struct CROP_PLOT* crops, int16_t crops_sz,
   struct DSEKAI_STATE* state
) {
   /* TODO: Warn that materials cannot be used directly. */
   return 0;
}

int8_t item_use_watercan(
   int16_t e_idx, int16_t owner_id,
   struct TILEMAP* t,
   struct ITEM* items, int16_t items_sz,
   struct MOBILE* mobiles, int16_t mobiles_sz,
   struct CROP_PLOT* crops, int16_t crops_sz,
   struct DSEKAI_STATE* state
) {
   /* TODO: Check for CROP_PLOTs that can be watered. */
   return 0;
}

int8_t item_use_hoe(
   int16_t e_idx, int16_t owner_id,
   struct TILEMAP* t,
   struct ITEM* items, int16_t items_sz,
   struct MOBILE* mobiles, int16_t mobiles_sz,
   struct CROP_PLOT* crops, int16_t crops_sz,
   struct DSEKAI_STATE* state
) {
   int8_t retval = ITEM_USED_SUCCESSFUL;
   int16_t x = 0,
      y = 0,
      i = 0,
      crop_idx = -1;
   struct MOBILE* user = NULL;

   if( 0 <= owner_id ) {
      user = &(mobiles[owner_id]);
   } else if( ITEM_OWNER_PLAYER == owner_id ) {
      user = &(state->player);
   }
   assert( NULL != user );

   x = user->coords.x + gc_mobile_x_offsets[mobile_get_dir( user )];
   y = user->coords.y + gc_mobile_y_offsets[mobile_get_dir( user )];

   /* TODO: Move this to crop.c. */

   for( i = 0 ; crops_sz > i ; i++ ) {
      /* Find an empty CROP_PLOT. */
      if( CROP_FLAG_ACTIVE != (CROP_FLAG_ACTIVE & crops[i].flags) ) {
         if( 0 > crop_idx ) {
            /* First empty encountered, use this one later. */
            crop_idx = i;
         }
         continue;
      }

      /* Crop plot is not empty, but does it collide? */
      if(
         crops[i].map_gid == t->gid &&
         x == crops[i].coords.x && y == crops[i].coords.y
      ) {
         retval = ITEM_USED_SUCCESSFUL;
         /* Destroy plot. */
         crops[i].flags &= ~CROP_FLAG_ACTIVE;
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
      (MEMORY_PTR)&(crops[crop_idx]), sizeof( struct CROP_PLOT ) );
   crops[crop_idx].map_gid = t->gid;
   crops[crop_idx].coords.x = x;
   crops[crop_idx].coords.y = y;
   crops[crop_idx].flags |= CROP_FLAG_ACTIVE;
   debug_printf( 2, "created crop plot %d at %d, %d on map %d",
      crop_idx,
      crops[crop_idx].coords.x, crops[crop_idx].coords.y,
      crops[crop_idx].map_gid );

cleanup:

   return retval;
}

int16_t item_exists_in_inventory(
   int16_t template_gid, int16_t owner_id,
   struct ITEM* items, int16_t items_sz,
   struct DSEKAI_STATE* state
) {
   int16_t i = 0;

   /* Determine if the recipient has one of these already. */
   for( i = 0 ; items_sz > i ; i++ ) {
      item_break_if_last( items, i );
      if(
         ITEM_FLAG_ACTIVE == (items[i].flags & ITEM_FLAG_ACTIVE) &&
         items[i].gid == template_gid &&
         items[i].owner == owner_id
      ) {
         debug_printf(
            2, "item gid %d exists in owner %d inventory at index: %d",
            template_gid, owner_id, i );
         goto cleanup;
      }
   }

   debug_printf( 2, "item gid %d does not exist in owner %d inventory!",
      template_gid, owner_id );
   i = ITEM_ERROR_NOT_FOUND;

cleanup:
 
   return i;
}

int16_t item_decr_or_delete(
   int16_t e_idx,
   struct ITEM* items, int16_t items_sz,
   struct DSEKAI_STATE* state,
) {

   assert( ITEM_FLAG_ACTIVE == (ITEM_FLAG_ACTIVE & items[e_idx].flags) );

   if( 1 < item_get_count_flag( &(items[e_idx]) ) ) {
      /* Reduce the item's count. */
      item_incr_count( &(items[e_idx]), -1 );
   } else {
      /* Delete the item. */
      items[e_idx].flags &= ~ITEM_FLAG_ACTIVE;
      e_idx = ITEM_ERROR_NOT_FOUND;
   }

   return e_idx;
}

int16_t item_stack_or_add(
   int16_t template_gid, int16_t owner_id,
   struct TILEMAP* t,
   struct ITEM* items, int16_t items_sz,
   struct DSEKAI_STATE* state
) {
   int16_t e_idx = 0,
      i = 0;
   struct ITEM* e_def = NULL;

   /* Find the item definition with the given GID in tilemap item templates. */
   for( i = 0 ; items_sz > i ; i++ ) {
      item_break_if_last( t->item_defs, i );
      if(
         t->item_defs[i].gid == template_gid &&
         ITEM_FLAG_ACTIVE == (ITEM_FLAG_ACTIVE & t->item_defs[i].flags)
      ) {
         e_def = &(t->item_defs[i]);
         debug_printf( 2, "found item with gid %d and owner %d at index: %d",
            template_gid, owner_id, i );
         break;
      }
   }

   if( NULL == e_def ) {
      error_printf( "could not find item with gid: %d", template_gid );
      e_idx = ITEM_ERROR_MISSING_TEMPLATE;
      goto cleanup;
   }

   /* Determine if we're stacking or adding. */
   e_idx = item_exists_in_inventory(
      template_gid, owner_id, items, items_sz, state );

   if(
      /* Found a dupe (stacking). */
      0 <= e_idx &&
      /* Room on a stack. */
      gc_items_max[item_get_type_flag( e_def )] >=
         item_get_count_flag( &(items[e_idx]) ) + 1
   ) {
      debug_printf( 2, "adding item %d to mobile %d stack (%d)",
         template_gid, owner_id, item_get_count_flag( &(items[e_idx]) ) );

   } else {
      /* Create item from template. */
      /* TODO: Check for full inventory vs ITEM_INVENTORY_MAX. */
      for( i = 0 ; items_sz > i ; i++ ) {
         /* Do NOT break on last item here, since we're just looking for a
          * free slot!
          */
         if( ITEM_FLAG_ACTIVE == (ITEM_FLAG_ACTIVE & items[i].flags) ) {
            /* Skip active items. */
            continue;
         }

         debug_printf(
            2, "creating item with gid %d and owner %d at index: %d",
            e_def->gid, owner_id, i );

         memory_copy_ptr( 
            (MEMORY_PTR)&(items[i]), (CONST_MEMORY_PTR)e_def,
            sizeof( struct ITEM ) );

         items[i].owner = owner_id;
         items[i].sprite_cache_id = -1;
         e_idx = i;

         /* Found and created the item, so quit. */
         break;
      }
   }

   /* Defs start with count of 0, or we found the e_idx above. */
   item_incr_count( &(items[e_idx]), 1 );

cleanup:

   if( 0 > e_idx ) {
      error_printf( "unable to give item!" );
   }

   return e_idx;
}

int16_t item_give_mobile(
   int16_t e_idx, int16_t owner_id, struct TILEMAP* t,
   struct ITEM* items, int16_t items_sz,
   struct MOBILE* mobiles, int16_t mobiles_sz,
   struct DSEKAI_STATE* state
) {
   int16_t e_dest_idx = 0;
   int16_t e_gid = 0;

   /* Grab the source item's GID for use later. */
   assert( ITEM_FLAG_ACTIVE == (ITEM_FLAG_ACTIVE & items[e_idx].flags) );
   e_gid = items[e_idx].gid;
   
   debug_printf( 2, "giving item at index %d with gid %d to owner: %d",
      e_idx, e_gid, owner_id );

   /* TODO: Check for full inventory vs ITEM_INVENTORY_MAX. */

   item_stack_or_add( e_gid, owner_id, t, items, items_sz, state );

   /* Since give was successful, remove former owner's copy. */
   item_decr_or_delete( e_idx, items, items_sz, state );

   return e_dest_idx;
}

int8_t item_drop(
   int16_t e_idx,
   struct TILEMAP* t,
   struct ITEM* items, int16_t items_sz,
   struct MOBILE* mobiles, int16_t mobiles_sz,
   struct DSEKAI_STATE* state
) {
   struct ITEM* e = NULL;

   assert( items_sz > e_idx );
   assert( 0 <= e_idx );
   e = &(items[e_idx]);
   assert( ITEM_FLAG_ACTIVE == (ITEM_FLAG_ACTIVE & e->flags) );

   if( ITEM_OWNER_NONE == e->owner ) {
      goto cleanup;
   }

   /* Set the item's new map coordinates based on previous owner. */
   if( ITEM_OWNER_PLAYER == e->owner ) {
      e->x = state->player.coords.x;
      e->y = state->player.coords.y;
   } else if( 0 <= e->owner && mobiles_sz > e->owner ) {
      e->x = mobiles[e->owner].coords.x;
      e->y = mobiles[e->owner].coords.y;
   }

   debug_printf( 2, "dropped item at %d, %d", e->x, e->y );

   e->owner = ITEM_OWNER_NONE;
   e->map_gid = t->gid;

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
   uint8_t x, uint8_t y, int16_t owner, struct TILEMAP* t,
   struct ITEM* items, int16_t items_sz,
   struct DSEKAI_STATE* state
) {
   int16_t i = 0;

   for( i = 0 ; items_sz > i ; i++ ) {
      /* item_break_if_last( items, i ); */
      if(
         /* Skip inactive items. */
         !(ITEM_FLAG_ACTIVE & items[i].flags) ||
         /* Skip owned items. */
         ITEM_OWNER_NONE != items[i].owner ||
         /* Skip items on other maps. */
         items[i].map_gid != t->gid
      ) {
         continue;
      }

      if( x == items[i].x && y == items[i].y ) {
         debug_printf( 2, "assigned owner %d to item %d at %d, %d",
            owner, i, x, y );
         /* TODO: Switch owner to use GID instead of index. */
         items[i].owner = owner;

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

   return i;
}

/*
void item_cleanup_orphans( struct DSEKAI_STATE* ) {
   int16_t i = 0;

   for( i = 0 ; DSEKAI_ITEMS_MAX > i ; i++ ) {
      if(
         ITEM_FLAG_ACTIVE == (ITEM_FLAG_ACTIVE & state->items[i].flags) &&
         0 <= state->items[i].owner
   }
}
*/

