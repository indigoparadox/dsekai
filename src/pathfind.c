
#include "dsekai.h"

/**
 * \return Index of the list node with the lowest total node cost.
 */
static
int8_t pathfind_list_lowest_f( struct PATHFIND_NODE* list, uint8_t list_sz ) {
   int8_t i = 0;
   uint8_t lowest_f_idx = 0;

   for( i = 0 ; list_sz > i ; i++ ) {
      if( list[lowest_f_idx].f > list[i].f ) {
         lowest_f_idx = i;
      }
   }

   pathfind_trace_printf( 1, "lowest cost open tile is %d, %d, cost is %d",
      list[lowest_f_idx].coords.x, list[lowest_f_idx].coords.y,
      list[lowest_f_idx].f );

   return lowest_f_idx;
}

static int8_t pathfind_list_test_add_child( 
   struct PATHFIND_NODE* adjacent,
   uint8_t iter_closed_idx,
   uint8_t tgt_x, uint8_t tgt_y,
   struct PATHFIND_NODE* open, uint8_t* open_sz,
   struct PATHFIND_NODE* closed, uint8_t closed_sz
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

   pathfind_trace_printf( 1, "> tile parent %d, %d, distance %d",
      closed[iter_closed_idx].coords.x,
      closed[iter_closed_idx].coords.y,
      closed[iter_closed_idx].g );

   adjacent->g = closed[iter_closed_idx].g + 1;
   pathfind_trace_printf( 1, "> tile distance is %d", adjacent->g );
   
   /* Use Manhattan heuristic since we can only move in 4 dirs. */
   adjacent->h = abs( a_x - tgt_x ) + abs( a_y - tgt_y );
   pathfind_trace_printf( 1, "> tile heuristic is %d", adjacent->h );

   adjacent->f = adjacent->g + adjacent->h;
   pathfind_trace_printf( 1, "> tile cost is %d", adjacent->f );

   for( i = 0 ; *open_sz > i ; i++ ) {
      /* Make sure adjacent is not already on the open list. */
      if( pathfind_cmp_eq( adjacent, &(open[i]) ) ) {
         return PATHFIND_ERROR_FARTHER;
      }
   }

   /* Add the child to the list. */
   dio_list_append(
      adjacent, open, *open_sz, PATHFIND_LIST_MAX, struct PATHFIND_NODE );
   if( 0 > g_dio_error ) {
      return LIST_ERROR_MAX;
   }

   return child_open_idx;
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
      open_sz = 0,
      tgt_reached = 0,
      i = 0;

   /* Zero out lists and nodes. */
   memory_zero_ptr(
      open, sizeof( struct PATHFIND_NODE ) * PATHFIND_LIST_MAX );
   memory_zero_ptr(
      closed, sizeof( struct PATHFIND_NODE ) * PATHFIND_LIST_MAX );
   memory_zero_ptr( &adjacent, sizeof( struct PATHFIND_NODE ) );
   
   /* Add the start node to the open list. */
   adjacent.coords.x = mover->coords.x;
   adjacent.coords.y = mover->coords.y;
   dio_list_append(
      &adjacent, open, open_sz, PATHFIND_LIST_MAX, struct PATHFIND_NODE );
   
   pathfind_trace_printf( 1, "pathfinding to %d, %d...", tgt_x, tgt_y );

   while( 0 < open_sz ) {
      iter_idx = pathfind_list_lowest_f( open, open_sz );

      /* Move the iter node to the closed list. */
      pathfind_trace_printf( 1,
         "moving %d, %d to closed list idx %d...",
         open[iter_idx].coords.x, open[iter_idx].coords.y, closed_sz );
      memory_copy_ptr(
         &(closed[closed_sz]), &(open[iter_idx]), 
         sizeof( struct PATHFIND_NODE ) );
      dio_list_remove( iter_idx, open, open_sz, struct PATHFIND_NODE );
      iter_idx = closed_sz;
      closed_sz++;
      
      /* Validate closed list size. */
      if( closed_sz >= PATHFIND_LIST_MAX ) {
         pathfind_trace_printf( "> pathfind stack exceeded" );
         return LIST_ERROR_MAX;
      }

      /* Check to see if we've reached the target. */
      if(
         closed[iter_idx].coords.x == tgt_x &&
         closed[iter_idx].coords.y == tgt_y
      ) {
         pathfind_trace_printf( 1, "> target reached!" );
         tgt_reached = 1;
         break;
      }

      /* Test and add each of the 4 adjacent tiles. */
      for( i = 0 ; 4 > i ; i++ ) {
         /* Don't wander off the map! */
         if(
            (0 == closed[iter_idx].coords.x && gc_mobile_x_offsets[i] == -1) ||
            (0 == closed[iter_idx].coords.y && gc_mobile_y_offsets[i] == -1) ||
            (255 == closed[iter_idx].coords.x && gc_mobile_x_offsets[i] == 1) ||
            (255 == closed[iter_idx].coords.y && gc_mobile_y_offsets[i] == 1)
         ) {
            pathfind_trace_printf( 1, "> skipping overflow tile!" );
            continue;
         }

         /* Setup tentative adjacent tile physical properties. */
         /* Scores will be calculated in pathfind_list_test_add_child(). */
         adjacent.coords.x = closed[iter_idx].coords.x + 
            gc_mobile_x_offsets[i];
         adjacent.coords.y = closed[iter_idx].coords.y +
            gc_mobile_y_offsets[i];
         adjacent.dir = i;
         pathfind_list_test_add_child(
            &adjacent, iter_idx, tgt_x, tgt_y, open, &open_sz,
            closed, closed_sz );
      }

   }

   if( tgt_reached ) {
      /* Return the next closest tile to the target. */
      pathfind_trace_printf( 1, "> tgt reached! %d, %d",
         closed[1].coords.x, closed[1].coords.y );
      return closed[1].dir;
   } else {
      pathfind_trace_printf( 1, "> blocked! (closed sz %d)", closed_sz );
   }

   return MOBILE_ERROR_BLOCKED;
}

