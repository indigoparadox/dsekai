
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
   /* TODO */
   return 0;
}

int8_t item_use_seed(
   struct ITEM* e, struct MOBILE* user, struct DSEKAI_STATE* state
) {
   int8_t crop_def_idx = -1,
      retval = -1;
   /* TODO: Check the list of CROP_PLOTs for a fertile, unseeded plot. */

   /* TODO: Check TILEMAP::crop_defs for ::CROP_DEF with same name. */

   if( 0 > crop_def_idx ) { 
      /* TODO: On-screen warning. */
      error_printf( "unable to grow this crop on the current tilemap" );
      retval = 0;
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
#endif

   return -1;
}

int8_t item_use_shovel(
   struct ITEM* e, struct MOBILE* user, struct DSEKAI_STATE* state
) {
   /* TODO */
   return 0;
}

int8_t item_use_axe(
   struct ITEM* e, struct MOBILE* user, struct DSEKAI_STATE* state
) {
   /* TODO */
   return 0;
}

int8_t item_use_editor(
   struct ITEM* e, struct MOBILE* user, struct DSEKAI_STATE* state
) {
#ifndef NO_ENGINE_EDITOR
   if( TILEMAP_FLAG_EDITABLE != (TILEMAP_FLAG_EDITABLE & state->map.flags) ) {

      /* TODO: Display warning message on-screen. */
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

   x = user->coords.x + gc_mobile_x_offsets[user->dir];
   y = user->coords.y + gc_mobile_y_offsets[user->dir];

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
            state->crops[i].map_name, state->map.name, TILEMAP_NAME_MAX ) &&
         x == state->crops[i].coords.x && y == state->crops[i].coords.y
      ) {
         /* TODO: Display warning on screen. */
         retval = 0;
         crop_idx = -1;
         error_printf( "crop plot already exists" );
         goto cleanup;
      }
   }

   if( 0 > crop_idx ) {
      /* TODO: Display warning on screen. */
      retval = 0;
      error_printf( "no available crop plots" );
      goto cleanup;
   }

   /* Create crop plot. */
   memory_zero_ptr( &(state->crops[crop_idx]), sizeof( struct CROP_PLOT ) );
   memory_strncpy_ptr( state->crops[crop_idx].map_name, state->map.name,
      TILEMAP_NAME_MAX );
   state->crops[crop_idx].coords.x = x;
   state->crops[crop_idx].coords.y = y;
   state->crops[crop_idx].flags |= CROP_FLAG_ACTIVE;
   debug_printf( 2, "created crop plot %d at %d, %d on map %s",
      crop_idx,
      state->crops[crop_idx].coords.x, state->crops[crop_idx].coords.y,
      state->crops[crop_idx].map_name );

cleanup:

   return retval;
}

void item_draw( const struct ITEM* i, int16_t screen_x, int16_t screen_y ) {
   /*
   int16_t x_offset = 0,
      y_offset = 0;

   graphics_blit_sprite_at(
      i->sprite,
      0, 0,
      (i->coords.x * SPRITE_W) - screen_x,
      (i->coords.y * SPRITE_H) - screen_y,
      SPRITE_W, SPRITE_H);
   */
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

      memory_copy_ptr( &(state->items[i]), e, sizeof( struct ITEM ) );

      state->items[i].owner = m_idx;

      /* Found and created the item, so quit. */
      break;
   }

   return 1;
}

