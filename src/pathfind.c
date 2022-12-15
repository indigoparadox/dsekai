
#include "dsekai.h"

int8_t pathfind_test_dir(
   struct MOBILE* mover, int8_t dir_move, uint8_t force,
   struct DSEKAI_STATE* state, struct TILEMAP* t
) {

   if( 
      !force
#ifndef NO_GUI
      && 0 < window_modal()
#endif /* !NO_GUI */
   ) {
      return MOBILE_ERROR_BLOCKED;
   }

   if(
      !force &&
      DSEKAI_FLAG_INPUT_BLOCKED == (DSEKAI_FLAG_INPUT_BLOCKED & state->flags)
   ) {
      return MOBILE_ERROR_BLOCKED;
   }
   
   mobile_set_dir( &(state->player), dir_move );

   if(
      !tilemap_collide( &(state->player), dir_move, t ) &&
      NULL == mobile_get_facing( &(state->player), t, state )
   ) {
      /* No blocking tiles or mobiles. */
      return dir_move;
   }

   return MOBILE_ERROR_BLOCKED;
}

static int8_t pathfind_list_add(
   uint8_t x, uint8_t y, uint8_t f, uint8_t g, uint8_t h,
   struct PATHFIND_NODE* list
) {
   int8_t i = 0;

   for( i = 0 ; PATHFIND_LIST_MAX > i ; i++ ) {
      if( 0 == list[i].active ) {
         /* Found an inactive node, so break early! */
         break;
      }
   }

   if( i >= PATHFIND_LIST_MAX ) {
      return PATHFIND_ERROR_MAX;
   }

   list[i].coords.x = x;
   list[i].coords.y = y;
   list[i].f = f;
   list[i].g = g;
   list[i].h = h;

   return i;
}

/**
 * \return Index of the list node with the lowest total node cost.
 */
static int8_t pathfind_list_lowest_f( struct PATHFIND_NODE* list ) {
   int8_t i = 0;
   uint8_t lowest_f_idx = 0;

   for( i = 0 ; PATHFIND_LIST_MAX > i ; i++ ) {
      if( 0 == list[i].active ) {
         /* Found an inactive node, so break early! */
         break;
      }

      if( list[lowest_f_idx].f > list[i].f ) {
         lowest_f_idx = i;
      }
   }

   if( i >= PATHFIND_LIST_MAX ) {
      return PATHFIND_ERROR_MAX;
   }

   return lowest_f_idx;
}

static int8_t pathfind_list_test_add_child( 
   struct PATHFIND_NODE* adjacent,
   uint8_t iter_closed_idx,
   uint8_t tgt_x, uint8_t tgt_y,
   struct PATHFIND_NODE* open, struct PATHFIND_NODE* closed,
   uint8_t closed_sz
) {
   uint8_t i = 0,
      child_open_idx = 0;
   int16_t a_x = adjacent->coords.x,
      a_y = adjacent->coords.y;

   pathfind_trace_printf( 1, "testing tile at %d, %d...",
      adjacent->coords.x, adjacent->coords.y );
   
   /* Make sure adjacent is not in the closed list. */
   for( i = 0 ; closed_sz > i ; i++ ) {
      if( pathfind_cmp_eq( adjacent, &(closed[i]) ) ) {
         pathfind_trace_printf( 1, "> tile is already closed!" );
         return PATHFIND_ERROR_CLOSED;
      }
   }

   /* Calculate adjacent pathfinding properties. */

   adjacent->g = closed[iter_closed_idx].g + 1;
   pathfind_trace_printf( 1, "> tile distance is %d", adjacent->g );
   
   /* Use Manhattan heuristic since we can only move in 4 dirs. */
   adjacent->h = abs( a_x - tgt_x ) + abs( a_y - tgt_y );
   pathfind_trace_printf( 1, "> tile heuristic is %d", adjacent->h );

   adjacent->f = adjacent->g + adjacent->h;
   pathfind_trace_printf( 1, "> tile cost is %d", adjacent->f );

   for( i = 0 ; PATHFIND_LIST_MAX > i ; i++ ) {
      if(
         open[i].active &&
         pathfind_cmp_eq( adjacent, &(open[i]) ) &&
         adjacent->g > open[i].g
      ) {
         return PATHFIND_ERROR_FARTHER;
      }
   }

   /* Add the child to the list. */
   pathfind_list_add(
      adjacent->coords.x, adjacent->coords.y,
      adjacent->f, adjacent->g, adjacent->h, open );

   return child_open_idx;
}

static int8_t pathfind_list_sz( struct PATHFIND_NODE* list ) {
   int8_t list_sz = 0;
   uint8_t i = 0;

   for( i = 0 ; PATHFIND_LIST_MAX > i ; i++ ) {
      if( list[i].active ) {
         list_sz++;
      }
   }
   
   return list_sz;
}

int8_t pathfind_start(
   struct MOBILE* mover, uint8_t tgt_x, uint8_t tgt_y,
   struct DSEKAI_STATE* state, struct TILEMAP* t
) {
   struct PATHFIND_NODE open[PATHFIND_LIST_MAX];
   struct PATHFIND_NODE closed[PATHFIND_LIST_MAX];
   struct PATHFIND_NODE adjacent;
   uint8_t iter_idx = 0,
      closed_sz = 0,
      tgt_reached = 0,
      i = 0;

   memory_zero_ptr(
      open, sizeof( struct PATHFIND_NODE ) * PATHFIND_LIST_MAX );
   
   /* Add the start node to the open list. */
   debug_printf( 1, "start" );
   pathfind_list_add(
      mover->coords.x, mover->coords.y, 0, 0, 0, open );

   while( 0 < pathfind_list_sz( open ) ) {
      iter_idx = pathfind_list_lowest_f( open );
      if( 0 > iter_idx ) {
         /* Error occurred, so pass it upwards! */
         return iter_idx;
      }

      /* Move the iter node to the closed list by copying it and disabling the
       * original.
       */
      memory_copy_ptr(
         &(closed[closed_sz]), &(open[iter_idx]), 
         sizeof( struct PATHFIND_NODE ) );
      open[iter_idx].active = 0;
      iter_idx = closed_sz;
      closed_sz++;

      /* Check to see if we've reached the target. */
      if(
         closed[iter_idx].coords.x == tgt_x &&
         closed[iter_idx].coords.y == tgt_y
      ) {
         tgt_reached = 1;
         break;
      }

      /* Test and add each of the 4 adjacent tiles. */
      for( i = 0 ; 4 > i ; i++ ) {
         adjacent.coords.x = closed[iter_idx].coords.x + 
            gc_mobile_x_offsets[i];
         adjacent.coords.y = closed[iter_idx].coords.y +
            gc_mobile_y_offsets[i];
         adjacent.active = 1;
         pathfind_list_test_add_child(
            &adjacent, iter_idx, tgt_x, tgt_y, open, closed, closed_sz );
      }

   }

   if( tgt_reached ) {
      /* TODO: Return the next closest tile to the target. */
      debug_printf( 1, "tgt reached! %d, %d",
         closed[1].coords.x, closed[1].coords.y );
   } else {
      debug_printf( 1, "blocked! (closed sz %d)", closed_sz );
   }

   return MOBILE_ERROR_BLOCKED;
}

