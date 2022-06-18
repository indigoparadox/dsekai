
#define ITEM_C
#include "dsekai.h"

int8_t item_use_none(
   int8_t e_idx, int8_t owner_id, struct DSEKAI_STATE* state
) {
   error_printf( "attempted to use invalid item" );
   return 0;
}

int8_t item_use_seed(
   int8_t e_idx, int8_t owner_id, struct DSEKAI_STATE* state
) {
   int8_t retval = -1;
   uint8_t x = 0,
      y = 0,
      crop_gid = 0;
   struct CROP_PLOT* plot = NULL;
   struct TILEMAP* t = NULL;
   struct MOBILE* user = NULL;

   /* Owner check. */
   if( 0 <= owner_id ) {
      user = &(state->mobiles[owner_id]);
   } else if( ITEM_OWNER_PLAYER == owner_id ) {
      user = &(state->player);
   }
   assert( NULL != user );

   crop_gid = item_get_data( e_idx, state );

   x = user->coords.x + gc_mobile_x_offsets[user->dir];
   y = user->coords.y + gc_mobile_y_offsets[user->dir];

   /* Get the plot in front of the user. */
   t = (struct TILEMAP*)memory_lock( state->map_handle );
   if( NULL == t ) {
      error_printf( "could not lock tilemap" );
      goto cleanup;
   }
   plot = crop_find_plot( t, x, y, state );
   if( NULL == plot ) {
#ifdef SCREEN_W
      window_prefab_system_dialog(
         "The soil is\ntoo firm!",
         WINDOW_PREFAB_DEFAULT_FG(), WINDOW_PREFAB_DEFAULT_BG() );
#endif /* SCREEN_W */
      error_printf( "could not find plot" );
      retval = ITEM_USED_FAILED;
      goto cleanup;
   }

   if( 0 < crop_plant( crop_gid, plot, t ) ) {
      /* Planting was successful. */
      item_decr_or_delete( e_idx, state );
   } else {
#ifdef SCREEN_W
      window_prefab_system_dialog(
         "This won't\ngrow here!",
         WINDOW_PREFAB_DEFAULT_FG(), WINDOW_PREFAB_DEFAULT_BG() );
#endif /* SCREEN_W */
      error_printf( "unable to plant seed" );
      retval = ITEM_USED_FAILED;
   }

cleanup:

   if( NULL != t ) {
      t = (struct TILEMAP*)memory_unlock( state->map_handle );
   }

   return retval;
}

int8_t item_use_food(
   int8_t e_idx, int8_t owner_id, struct DSEKAI_STATE* state
) {
   int8_t anim_idx = 0;
   char num_str[10];
   struct MOBILE* user = NULL;
   uint8_t food_val = 0;

   /* Owner check. */
   if( 0 <= owner_id ) {
      user = &(state->mobiles[owner_id]);
   } else if( ITEM_OWNER_PLAYER == owner_id ) {
      user = &(state->player);
   }
   assert( NULL != user );

   food_val = item_get_data( e_idx, state );

   item_decr_or_delete( e_idx, state );

   /* TODO: Different types of food for different stats? */
   user->hp += food_val;

   dio_snprintf( num_str, 10, "+%d", food_val );

#if defined( DEPTH_VGA ) || defined( DEPTH_CGA )
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
#endif /* DEPTH_VGA || DEPTH_CGA */

   return ITEM_USED_SUCCESSFUL;
}

int8_t item_use_shovel(
   int8_t e_idx, int8_t owner_id, struct DSEKAI_STATE* state
) {
   /* TODO: Implement digging. */
   return ITEM_USED_FAILED;
}

int8_t item_use_editor(
   int8_t e_idx, int8_t owner_id, struct DSEKAI_STATE* state
) {
#ifndef NO_ENGINE_EDITOR
   struct TILEMAP* t = NULL;

   t = (struct TILEMAP*)memory_lock( state->map_handle );
   assert( NULL != t );

   if( TILEMAP_FLAG_EDITABLE != (TILEMAP_FLAG_EDITABLE & t->flags) ) {

      /* TODO: Display warning message on-screen. */
#ifdef SCREEN_W
      window_prefab_system_dialog(
         "This map cannot\nbe edited!",
         WINDOW_PREFAB_DEFAULT_FG(), WINDOW_PREFAB_DEFAULT_BG() );
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

   t = (struct TILEMAP*)memory_unlock( state->map_handle );
#endif /* !NO_ENGINE_EDITOR */

   return -1;
}

int8_t item_use_material(
   int8_t e_idx, int8_t owner_id, struct DSEKAI_STATE* state
) {
   /* TODO: Warn that materials cannot be used directly. */
   return 0;
}

int8_t item_use_watercan(
   int8_t e_idx, int8_t owner_id, struct DSEKAI_STATE* state
) {
   /* TODO: Check for CROP_PLOTs that can be watered. */
   return 0;
}

int8_t item_use_hoe(
   int8_t e_idx, int8_t owner_id, struct DSEKAI_STATE* state
) {
   int8_t i = 0,
      crop_idx = -1,
      retval = ITEM_USED_SUCCESSFUL;
   int16_t x = 0, y = 0;
   struct TILEMAP* t = NULL;
   struct MOBILE* user = NULL;

   if( 0 <= owner_id ) {
      user = &(state->mobiles[owner_id]);
   } else if( ITEM_OWNER_PLAYER == owner_id ) {
      user = &(state->player);
   }
   assert( NULL != user );

   x = user->coords.x + gc_mobile_x_offsets[user->dir];
   y = user->coords.y + gc_mobile_y_offsets[user->dir];

   /* TODO: Move this to crop.c. */

   t = (struct TILEMAP*)memory_lock( state->map_handle );
   assert( NULL != t );

   for( i = 0 ; DSEKAI_CROPS_MAX > i ; i++ ) {
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
         0 == memory_strncmp_ptr(
            state->crops[i].map_name, t->name, TILEMAP_NAME_MAX ) &&
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
      window_prefab_system_dialog(
         "Too many crops\nalready planted!",
         WINDOW_PREFAB_DEFAULT_FG(), WINDOW_PREFAB_DEFAULT_BG() );
#endif /* SCREEN_W */
      retval = ITEM_USED_FAILED;
      error_printf( "no available crop plots" );
      goto cleanup;
   }

   /* Create crop plot. */
   memory_zero_ptr(
      (MEMORY_PTR)&(state->crops[crop_idx]), sizeof( struct CROP_PLOT ) );
   memory_strncpy_ptr( state->crops[crop_idx].map_name, t->name,
      TILEMAP_NAME_MAX );
   state->crops[crop_idx].coords.x = x;
   state->crops[crop_idx].coords.y = y;
   state->crops[crop_idx].flags |= CROP_FLAG_ACTIVE;
   debug_printf( 2, "created crop plot %d at %d, %d on map %s",
      crop_idx,
      state->crops[crop_idx].coords.x, state->crops[crop_idx].coords.y,
      state->crops[crop_idx].map_name );

cleanup:

   t = (struct TILEMAP*)memory_unlock( state->map_handle );

   return retval;
}

uint8_t item_get_data( int8_t e_idx, struct DSEKAI_STATE* state ) {
   uint8_t data_out = 0;
   struct ITEM* items = NULL;

   assert( e_idx < DSEKAI_ITEMS_MAX );
   assert( 0 <= e_idx );

   /* Handle locking. */
   items = (struct ITEM*)memory_lock( state->items_handle );
   assert( NULL != items );
   
   assert( ITEM_FLAG_ACTIVE == (ITEM_FLAG_ACTIVE & items[e_idx].flags) );

   data_out = items[e_idx].data;

   items = (struct ITEM*)memory_unlock( state->items_handle );

   return data_out;
}

uint8_t item_get_type( int8_t e_idx, struct DSEKAI_STATE* state ) {
   uint8_t type_out = 0;
   struct ITEM* items = NULL;

   assert( e_idx < DSEKAI_ITEMS_MAX );
   assert( 0 <= e_idx );

   /* Handle locking. */
   items = (struct ITEM*)memory_lock( state->items_handle );
   assert( NULL != items );
   
   assert( ITEM_FLAG_ACTIVE == (ITEM_FLAG_ACTIVE & items[e_idx].flags) );

   type_out = items[e_idx].type;

   items = (struct ITEM*)memory_unlock( state->items_handle );

   return type_out;
}

uint8_t item_get_flags( int8_t e_idx, struct DSEKAI_STATE* state ) {
   uint8_t flags_out = 0;
   struct ITEM* items = NULL;

   assert( e_idx < DSEKAI_ITEMS_MAX );
   assert( 0 <= e_idx );

   /* Handle locking. */
   items = (struct ITEM*)memory_lock( state->items_handle );
   assert( NULL != items );
   
   flags_out = items[e_idx].flags;

   items = (struct ITEM*)memory_unlock( state->items_handle );

   return flags_out;
}

int8_t item_get_owner( int8_t e_idx, struct DSEKAI_STATE* state ) {
   int8_t owner_out = 0;
   struct ITEM* items = NULL;

   assert( e_idx < DSEKAI_ITEMS_MAX );
   assert( 0 <= e_idx );

   /* Handle locking. */
   items = (struct ITEM*)memory_lock( state->items_handle );
   assert( NULL != items );
   
   assert( ITEM_FLAG_ACTIVE == (ITEM_FLAG_ACTIVE & items[e_idx].flags) );

   owner_out = items[e_idx].owner;

   items = (struct ITEM*)memory_unlock( state->items_handle );

   return owner_out;
}

int8_t item_exists_in_inventory(
   int16_t template_gid, int8_t owner_id, struct DSEKAI_STATE* state
) {
   int8_t i = 0;
   struct ITEM* items = NULL;

   items = (struct ITEM*)memory_lock( state->items_handle );
   assert( NULL != items );

   /* Determine if the recipient has one of these already. */
   for( i = 0 ; DSEKAI_ITEMS_MAX > i ; i++ ) {
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
 
   if( NULL != items ) {
      items = (struct ITEM*)memory_unlock( state->items_handle );
   }

   return i;
}

int8_t item_decr_or_delete( int8_t e_idx, struct DSEKAI_STATE* state ) {
   struct ITEM* items = NULL;

   items = (struct ITEM*)memory_lock( state->items_handle );
   assert( NULL != items );
   assert( ITEM_FLAG_ACTIVE == (ITEM_FLAG_ACTIVE & items[e_idx].flags) );

   if( 1 < items[e_idx].count ) {
      /* Reduce the item's count. */
      items[e_idx].count--;
   } else {
      /* Delete the item. */
      items[e_idx].flags &= ~ITEM_FLAG_ACTIVE;
      e_idx = ITEM_ERROR_NOT_FOUND;
   }

   items = (struct ITEM*)memory_unlock( state->items_handle );

   return e_idx;
}

int8_t item_stack_or_add(
   int16_t template_gid, int8_t owner_id,
   struct TILEMAP* t, struct DSEKAI_STATE* state
) {
   int8_t e_idx = 0,
      i = 0;
   struct ITEM* e_def = NULL,
      * items = NULL;

   /* Find the item definition with the given GID in tilemap item templates. */
   for( i = 0 ; TILEMAP_ITEMS_MAX > i ; i++ ) {
      if(
         t->items[i].gid == template_gid &&
         ITEM_FLAG_ACTIVE == (ITEM_FLAG_ACTIVE & t->items[i].flags)
      ) {
         e_def = &(t->items[i]);
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
   e_idx = item_exists_in_inventory( template_gid, owner_id, state );

   items = (struct ITEM*)memory_lock( state->items_handle );
   assert( NULL != items );
   
   if( 0 <= e_idx ) {
      /* Found a dupe (stacking). */
      if( gc_items_max[e_def->type] >= items[e_idx].count + 1 ) {
         items[e_idx].count++;
         debug_printf( 2, "adding item %d to mobile %d stack (%d)",
            template_gid, owner_id, items[e_idx].count );
      } else {
         error_printf( "unable to give item %d to mobile %d: duplicate",
            template_gid, owner_id );
         e_idx = ITEM_ERROR_DUPLICATE;
      }

   } else {
      /* Create item from template. */
      /* TODO: Check for full inventory vs ITEM_INVENTORY_MAX. */
      for( i = 0 ; DSEKAI_ITEMS_MAX > i ; i++ ) {
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
         e_idx = i;

         /* Found and created the item, so quit. */
         break;
      }
   }

cleanup:

   if( NULL != items ) {
      items = (struct ITEM*)memory_unlock( state->items_handle );
   }

   if( 0 > e_idx ) {
      error_printf( "unable to give item!" );
   }

   return e_idx;
}

int8_t item_give_mobile(
   int8_t e_idx, int8_t owner_id, struct TILEMAP* t, struct DSEKAI_STATE* state
) {
   int8_t e_dest_idx = 0;
   int16_t e_gid = 0;
   struct ITEM* items = NULL;

   /* Grab the source item's GID for use later. */
   items = (struct ITEM*)memory_lock( state->items_handle );
   assert( NULL != items );
   assert( ITEM_FLAG_ACTIVE == (ITEM_FLAG_ACTIVE & items[e_idx].flags) );
   e_gid = items[e_idx].gid;
   items = (struct ITEM*)memory_unlock( state->items_handle );
   
   debug_printf( 2, "giving item at index %d with gid %d to owner: %d",
      e_idx, e_gid, owner_id );

   /* TODO: Check for full inventory vs ITEM_INVENTORY_MAX. */

   item_stack_or_add( e_gid, owner_id, t, state );

   /* Since give was successful, remove former owner's copy. */
   item_decr_or_delete( e_idx, state );

   return e_dest_idx;
}

int8_t item_drop(
   int8_t e_idx, struct TILEMAP* t, struct DSEKAI_STATE* state
) {
   struct ITEM* e = NULL,
      * items = NULL;

   items = (struct ITEM*)memory_lock( state->items_handle );
   assert( NULL != items );

   assert( DSEKAI_ITEMS_MAX > e_idx );
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
   } else if( 0 <= e->owner && DSEKAI_MOBILES_MAX > e->owner ) {
      e->x = state->mobiles[e->owner].coords.x;
      e->y = state->mobiles[e->owner].coords.y;
   }

   debug_printf( 2, "dropped item at %d, %d", e->x, e->y );

   e->owner = ITEM_OWNER_NONE;

   memory_strncpy_ptr( e->map_name, t->name, TILEMAP_NAME_MAX );

#if 0
   /* Set tile as dirty. */
   t = (struct TILEMAP*)memory_lock( state->map_handle );
   if( NULL != t ) {
      tilemap_set_dirty( item->x, item->y, t );
      t = (struct TILEMAP*)memory_unlock( state->map_handle );
   }
#endif

cleanup:

   if( NULL != items ) {
      items = (struct ITEM*)memory_unlock( state->items_handle );
   }

   return e_idx;
}

int8_t item_pickup_xy(
   uint8_t x, uint8_t y, int8_t owner, struct TILEMAP* t,
   struct DSEKAI_STATE* state
) {
   int8_t i = 0;
   struct ITEM* items = NULL;

   items = (struct ITEM*)memory_lock( state->items_handle );
   assert( NULL != items );

   for( i = 0 ; DSEKAI_ITEMS_MAX > i ; i++ ) {
      if(
         /* Skip inactive items. */
         !(ITEM_FLAG_ACTIVE & items[i].flags) ||
         /* Skip owned items. */
         ITEM_OWNER_NONE != items[i].owner ||
         /* Skip items on other maps. */
         0 != memory_strncmp_ptr(
            items[i].map_name, t->name, TILEMAP_NAME_MAX )
      ) {
         continue;
      }

      if( x == items[i].x && y == items[i].y ) {
         debug_printf( 2, "assigned owner %d to item %d at %d, %d",
            owner, i, x, y );
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

   items = (struct ITEM*)memory_unlock( state->items_handle );

   return i;
}

/*
void item_cleanup_orphans( struct DSEKAI_STATE* ) {
   int8_t i = 0;

   for( i = 0 ; DSEKAI_ITEMS_MAX > i ; i++ ) {
      if(
         ITEM_FLAG_ACTIVE == (ITEM_FLAG_ACTIVE & state->items[i].flags) &&
         0 <= state->items[i].owner
   }
}
*/

