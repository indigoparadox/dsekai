
#define ITEM_C
#include "dsekai.h"

static void item_consume( struct ITEM* e ) {
   if( 1 < e->count ) {
      /* Reduce the item's count. */
      e->count--;
   } else {
      /* Delete the item. */
      e->flags &= ~ITEM_FLAG_ACTIVE;
   }
}

int8_t item_use_none(
   struct ITEM* e, struct MOBILE* user, struct DSEKAI_STATE* state
) {
   error_printf( "attempted to use invalid item" );
   return 0;
}

int8_t item_use_seed(
   struct ITEM* e, struct MOBILE* user, struct DSEKAI_STATE* state
) {
   int8_t retval = -1;
   uint8_t x = 0,
      y = 0;
   struct CROP_PLOT* plot = NULL;
   struct TILEMAP* map = NULL;

   x = user->coords.x + gc_mobile_x_offsets[user->dir];
   y = user->coords.y + gc_mobile_y_offsets[user->dir];

   /* Get the plot in front of the user. */
   map = (struct TILEMAP*)memory_lock( state->map_handle );
   if( NULL == map ) {
      error_printf( "could not lock tilemap" );
      goto cleanup;
   }
   plot = crop_find_plot( map, x, y, state );
   if( NULL == plot ) {
#ifdef SCREEN_W
      window_prefab_system_dialog(
         "The soil is\ntoo firm!", state, map,
         WINDOW_PREFAB_DEFAULT_FG(), WINDOW_PREFAB_DEFAULT_BG() );
#endif /* SCREEN_W */
      error_printf( "could not find plot" );
      retval = -1;
      goto cleanup;
   }

   if( 0 < crop_plant( e->data, plot, map ) ) {
      /* Planting was successful. */
      item_consume( e );
   } else {
#ifdef SCREEN_W
      window_prefab_system_dialog(
         "This won't\ngrow here!", state, map,
         WINDOW_PREFAB_DEFAULT_FG(), WINDOW_PREFAB_DEFAULT_BG() );
#endif /* SCREEN_W */
      error_printf( "unable to plant seed" );
      retval = -1;
   }

cleanup:

   if( NULL != map ) {
      map = (struct TILEMAP*)memory_unlock( state->map_handle );
   }

   return retval;
}

int8_t item_use_food(
   struct ITEM* e, struct MOBILE* user, struct DSEKAI_STATE* state
) {
   int8_t anim_idx = 0;
   char num_str[10];

   item_consume( e );

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
   struct ITEM* e, struct MOBILE* user, struct DSEKAI_STATE* state
) {
   /* TODO: Implement digging. */
   return 0;
}

int8_t item_use_editor(
   struct ITEM* e, struct MOBILE* user, struct DSEKAI_STATE* state
) {
#ifndef NO_ENGINE_EDITOR
   struct TILEMAP* map = NULL;

   map = (struct TILEMAP*)memory_lock( state->map_handle );
   if( NULL == map ) {
      error_printf( "could not lock tilemap" );
      return -1;
   }

   if( TILEMAP_FLAG_EDITABLE != (TILEMAP_FLAG_EDITABLE & map->flags) ) {

      /* TODO: Display warning message on-screen. */
#ifdef SCREEN_W
      window_prefab_system_dialog(
         "This map cannot\nbe edited!", state, map,
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

   map = (struct TILEMAP*)memory_unlock( state->map_handle );
#endif /* !NO_ENGINE_EDITOR */

   return -1;
}

int8_t item_use_material(
   struct ITEM* e, struct MOBILE* user, struct DSEKAI_STATE* state
) {
   /* TODO: Warn that materials cannot be used directly. */
   return 0;
}

int8_t item_use_watercan(
   struct ITEM* e, struct MOBILE* user, struct DSEKAI_STATE* state
) {
   /* TODO: Check for CROP_PLOTs that can be watered. */
   return 0;
}

int8_t item_use_hoe(
   struct ITEM* e, struct MOBILE* user, struct DSEKAI_STATE* state
) {
   int8_t i = 0,
      crop_idx = -1,
      retval = -1;
   int16_t x = 0, y = 0;
   struct TILEMAP* map = NULL;

   x = user->coords.x + gc_mobile_x_offsets[user->dir];
   y = user->coords.y + gc_mobile_y_offsets[user->dir];

   /* TODO: Move this to crop.c. */

   map = (struct TILEMAP*)memory_lock( state->map_handle );
   if( NULL == map ) {
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
            state->crops[i].map_name, map->name, TILEMAP_NAME_MAX ) &&
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
         "Too many crops\nalready planted!", state, map,
         WINDOW_PREFAB_DEFAULT_FG(), WINDOW_PREFAB_DEFAULT_BG() );
#endif /* SCREEN_W */
      retval = 0;
      error_printf( "no available crop plots" );
      goto cleanup;
   }

   /* Create crop plot. */
   memory_zero_ptr(
      (MEMORY_PTR)&(state->crops[crop_idx]), sizeof( struct CROP_PLOT ) );
   memory_strncpy_ptr( state->crops[crop_idx].map_name, map->name,
      TILEMAP_NAME_MAX );
   state->crops[crop_idx].coords.x = x;
   state->crops[crop_idx].coords.y = y;
   state->crops[crop_idx].flags |= CROP_FLAG_ACTIVE;
   debug_printf( 2, "created crop plot %d at %d, %d on map %s",
      crop_idx,
      state->crops[crop_idx].coords.x, state->crops[crop_idx].coords.y,
      state->crops[crop_idx].map_name );

cleanup:

   map = (struct TILEMAP*)memory_unlock( state->map_handle );

   return retval;
}

int8_t item_give_mobile(
   struct ITEM* e, struct MOBILE* m, struct DSEKAI_STATE* state
) {
   int8_t m_idx = ITEM_OWNER_PLAYER,
      i = 0;

   /* TODO: Check for full inventory. */

   assert( ITEM_FLAG_ACTIVE == (ITEM_FLAG_ACTIVE & e->flags) );

   /* Translate MEMORY_PTR to mobile index. Default to player on failure. */
   for( i = 0 ; DSEKAI_MOBILES_MAX > i ; i++ ) {
      if( m == &(state->mobiles[i]) ) {
         m_idx = i;
         break;
      }
   }

   /* Determine if the recipient has one of these already. */
   for( i = 0 ; DSEKAI_ITEMS_MAX > i ; i++ ) {
      if(
         ITEM_FLAG_ACTIVE == (state->items[i].flags & ITEM_FLAG_ACTIVE) &&
         state->items[i].gid == e->gid &&
         state->items[i].owner == m_idx
      ) {
         /* Found a dupe. */
         if( gc_items_max[e->type] > state->items[i].count + e->count ) {
            state->items[i].count++;
            debug_printf( 3, "adding item %d to mobile %d stack (%d)",
               e->gid, m_idx, state->items[i].count );
            return 1;
         } else {
            debug_printf( 3, "unable to give item %d to mobile %d: duplicate",
               e->gid, m_idx );
            return ITEM_ERROR_DUPLICATE;
         }
      }
   }

   debug_printf( 3, "giving item %s (%d) to mobile %d",
      e->name, e->gid, m_idx );

   /* Create item from template. */
   for( i = 0 ; DSEKAI_ITEMS_MAX > i ; i++ ) {
      if( ITEM_FLAG_ACTIVE == (ITEM_FLAG_ACTIVE & state->items[i].flags) ) {
         /* Skip active items. */
         continue;
      }

      memory_copy_ptr( 
         (MEMORY_PTR)&(state->items[i]), (CONST_MEMORY_PTR)e,
         sizeof( struct ITEM ) );

      state->items[i].owner = m_idx;

      /* Found and created the item, so quit. */
      break;
   }

   return 1;
}

int8_t item_drop(
   struct ITEM* item, struct TILEMAP* t, struct DSEKAI_STATE* state
) {

   if( ITEM_OWNER_NONE == item->owner ) {
      return 0;
   }

   /* Set the item's new map coordinates based on previous owner. */
   if( ITEM_OWNER_PLAYER == item->owner ) {
      item->x = state->player.coords.x;
      item->y = state->player.coords.y;
   } else if( 0 <= item->owner && DSEKAI_MOBILES_MAX > item->owner ) {
      item->x = state->mobiles[item->owner].coords.x;
      item->y = state->mobiles[item->owner].coords.y;
   }

   debug_printf( 2, "dropped item at %d, %d", item->x, item->y );

   item->owner = ITEM_OWNER_NONE;

   memory_strncpy_ptr( item->map_name, t->name, TILEMAP_NAME_MAX );

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

