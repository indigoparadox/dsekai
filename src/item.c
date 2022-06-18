
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
      y = 0;
   struct CROP_PLOT* plot = NULL;
   struct TILEMAP* t = NULL;
   struct MOBILE* user = NULL;
   struct ITEM* e = NULL;

   if( 0 <= owner_id ) {
      user = &(state->mobiles[owner_id]);
   } else if( ITEM_OWNER_PLAYER == owner_id ) {
      user = &(state->player);
   }
   assert( NULL != user );

   assert( e_idx < DSEKAI_ITEMS_MAX );
   assert( 0 <= e_idx );
   e = &(state->items[e_idx]);
   assert( ITEM_FLAG_ACTIVE == (ITEM_FLAG_ACTIVE & e->flags) );

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
         "The soil is\ntoo firm!", state, t,
         WINDOW_PREFAB_DEFAULT_FG(), WINDOW_PREFAB_DEFAULT_BG() );
#endif /* SCREEN_W */
      error_printf( "could not find plot" );
      retval = -1;
      goto cleanup;
   }

   if( 0 < crop_plant( e->data, plot, t ) ) {
      /* Planting was successful. */
      item_decr_or_delete( e->gid, e->owner, state );
   } else {
#ifdef SCREEN_W
      window_prefab_system_dialog(
         "This won't\ngrow here!", state, t,
         WINDOW_PREFAB_DEFAULT_FG(), WINDOW_PREFAB_DEFAULT_BG() );
#endif /* SCREEN_W */
      error_printf( "unable to plant seed" );
      retval = -1;
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
   struct ITEM* e = NULL;

   if( 0 <= owner_id ) {
      user = &(state->mobiles[owner_id]);
   } else if( ITEM_OWNER_PLAYER == owner_id ) {
      user = &(state->player);
   }
   assert( NULL != user );

   assert( e_idx < DSEKAI_ITEMS_MAX );
   assert( 0 <= e_idx );
   e = &(state->items[e_idx]);
   assert( ITEM_FLAG_ACTIVE == (ITEM_FLAG_ACTIVE & e->flags) );

   item_decr_or_delete( e->gid, e->owner, state );

   user->hp += e->data;

   dio_snprintf( num_str, 10, "+%d", e->data );

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

   return -1;
}

int8_t item_use_shovel(
   int8_t e_idx, int8_t owner_id, struct DSEKAI_STATE* state
) {
   /* TODO: Implement digging. */
   return 0;
}

int8_t item_use_editor(
   int8_t e_idx, int8_t owner_id, struct DSEKAI_STATE* state
) {
#ifndef NO_ENGINE_EDITOR
   struct TILEMAP* t = NULL;

   t = (struct TILEMAP*)memory_lock( state->map_handle );
   if( NULL == t ) {
      error_printf( "could not lock tilemap" );
      return -1;
   }

   if( TILEMAP_FLAG_EDITABLE != (TILEMAP_FLAG_EDITABLE & t->flags) ) {

      /* TODO: Display warning message on-screen. */
#ifdef SCREEN_W
      window_prefab_system_dialog(
         "This map cannot\nbe edited!", state, t,
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
      retval = -1;
   int16_t x = 0, y = 0;
   struct TILEMAP* t = NULL;
   struct MOBILE* user = NULL;
   struct ITEM* e = NULL;

   if( 0 <= owner_id ) {
      user = &(state->mobiles[owner_id]);
   } else if( ITEM_OWNER_PLAYER == owner_id ) {
      user = &(state->player);
   }
   assert( NULL != user );

   assert( e_idx < DSEKAI_ITEMS_MAX );
   assert( 0 <= e_idx );
   e = &(state->items[e_idx]);
   assert( ITEM_FLAG_ACTIVE == (ITEM_FLAG_ACTIVE & e->flags) );

   x = user->coords.x + gc_mobile_x_offsets[user->dir];
   y = user->coords.y + gc_mobile_y_offsets[user->dir];

   /* TODO: Move this to crop.c. */

   t = (struct TILEMAP*)memory_lock( state->map_handle );
   if( NULL == t ) {
      error_printf( "could not lock tilemap" );
      retval = 0;
      goto cleanup;
   }

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
         retval = -1;
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
         "Too many crops\nalready planted!", state, t,
         WINDOW_PREFAB_DEFAULT_FG(), WINDOW_PREFAB_DEFAULT_BG() );
#endif /* SCREEN_W */
      retval = 0;
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

int8_t item_exists_in_inventory(
   int16_t template_gid, int8_t owner_id, struct DSEKAI_STATE* state
) {
   int8_t i = 0;

   /* Determine if the recipient has one of these already. */
   for( i = 0 ; DSEKAI_ITEMS_MAX > i ; i++ ) {
      if(
         ITEM_FLAG_ACTIVE == (state->items[i].flags & ITEM_FLAG_ACTIVE) &&
         state->items[i].gid == template_gid &&
         state->items[i].owner == owner_id
      ) {
         debug_printf(
            2, "item gid %d exists in owner %d inventory at index: %d",
            template_gid, owner_id, i );
         return i;
      }
   }

   debug_printf( 2, "item gid %d does not exist in owner %d inventory!",
      template_gid, owner_id );
 
   return ITEM_ERROR_NOT_FOUND;
}

int8_t item_decr_or_delete(
   int16_t template_gid, int8_t owner_id, struct DSEKAI_STATE* state
) {
   struct ITEM* e = NULL;
   int8_t e_idx = ITEM_ERROR_NOT_FOUND;

   e_idx = item_exists_in_inventory( template_gid, owner_id, state );
   if( 0 > e_idx ) {
      goto cleanup;
   }
   e = &(state->items[e_idx]);

   assert( ITEM_FLAG_ACTIVE == (ITEM_FLAG_ACTIVE & e->flags) );

   if( 1 < e->count ) {
      /* Reduce the item's count. */
      e->count--;
   } else {
      /* Delete the item. */
      e->flags &= ~ITEM_FLAG_ACTIVE;
      e_idx = ITEM_ERROR_NOT_FOUND;
   }

cleanup:
   return e_idx;
}

int8_t item_stack_or_add(
   int16_t template_gid, int8_t owner_id,
   struct TILEMAP* t, struct DSEKAI_STATE* state
) {
   int8_t e_idx = 0,
      i = 0;
   struct ITEM* e_def = NULL;

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

   if( 0 <= e_idx ) {
      /* Found a dupe (stacking). */
      if( gc_items_max[e_def->type] >= state->items[e_idx].count + 1 ) {
         state->items[e_idx].count++;
         debug_printf( 2, "adding item %d to mobile %d stack (%d)",
            template_gid, owner_id, state->items[e_idx].count );
      } else {
         error_printf( "unable to give item %d to mobile %d: duplicate",
            template_gid, owner_id );
         e_idx = ITEM_ERROR_DUPLICATE;
      }

   } else {
      /* Create item from template. */
      /* TODO: Check for full inventory vs ITEM_INVENTORY_MAX. */
      for( i = 0 ; DSEKAI_ITEMS_MAX > i ; i++ ) {
         if( ITEM_FLAG_ACTIVE == (ITEM_FLAG_ACTIVE & state->items[i].flags) ) {
            /* Skip active items. */
            continue;
         }

         debug_printf(
            2, "creating item with gid %d and owner %d at index: %d",
            e_def->gid, owner_id, i );

         memory_copy_ptr( 
            (MEMORY_PTR)&(state->items[i]), (CONST_MEMORY_PTR)e_def,
            sizeof( struct ITEM ) );

         state->items[i].owner = owner_id;
         e_idx = i;

         /* Found and created the item, so quit. */
         break;
      }
   }

cleanup:

   if( 0 > e_idx ) {
      error_printf( "unable to give item!" );
   }

   return e_idx;
}

int8_t item_give_mobile(
   int8_t e_idx, int8_t owner_id, struct TILEMAP* t, struct DSEKAI_STATE* state
) {
   int8_t e_dest_idx = 0;
   struct ITEM* e = NULL;

   assert( DSEKAI_ITEMS_MAX > e_idx );
   assert( 0 <= e_idx );

   e = &(state->items[e_idx]);

   debug_printf( 2, "giving item at index %d to owner: %d",
      e_idx, owner_id );

   assert( ITEM_FLAG_ACTIVE == (ITEM_FLAG_ACTIVE & e->flags) );

   /* TODO: Check for full inventory vs ITEM_INVENTORY_MAX. */

   item_stack_or_add( e->gid, owner_id, t, state );

   /* Since give was successful, remove former owner's copy. */
   item_decr_or_delete( e->gid, e->owner, state );

   return e_dest_idx;
}

int8_t item_drop(
   int8_t e_idx, struct TILEMAP* t, struct DSEKAI_STATE* state
) {
   struct ITEM* e = NULL;

   assert( DSEKAI_ITEMS_MAX > e_idx );
   assert( 0 <= e_idx );
   e = &(state->items[e_idx]);
   assert( ITEM_FLAG_ACTIVE == (ITEM_FLAG_ACTIVE & e->flags) );

   if( ITEM_OWNER_NONE == e->owner ) {
      return 0;
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

   return 1;
}

int8_t item_pickup_xy(
   uint8_t x, uint8_t y, int8_t owner, struct TILEMAP* t,
   struct DSEKAI_STATE* state
) {
   int8_t i = 0;

   for( i = 0 ; DSEKAI_ITEMS_MAX > i ; i++ ) {
      if(
         /* Skip inactive items. */
         !(ITEM_FLAG_ACTIVE & state->items[i].flags) ||
         /* Skip owned items. */
         ITEM_OWNER_NONE != state->items[i].owner ||
         /* Skip items on other maps. */
         0 != memory_strncmp_ptr(
            state->items[i].map_name, t->name, TILEMAP_NAME_MAX )
      ) {
         continue;
      }

      if( x == state->items[i].x && y == state->items[i].y ) {
         debug_printf( 2, "assigned owner %d to item %d at %d, %d",
            owner, i, x, y );
         state->items[i].owner = owner;

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
   int8_t i = 0;

   for( i = 0 ; DSEKAI_ITEMS_MAX > i ; i++ ) {
      if(
         ITEM_FLAG_ACTIVE == (ITEM_FLAG_ACTIVE & state->items[i].flags) &&
         0 <= state->items[i].owner
   }
}
*/

