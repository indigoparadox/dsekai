
#include "dsekai.h"

#include <stdlib.h> /* For abs() */

static MERROR_RETVAL pathfind_list_append(
   struct PATHFIND_NODE* list, size_t* p_list_sz,
   struct PATHFIND_NODE* node
) {
   MERROR_RETVAL retval = MERROR_OK;

   if( *p_list_sz + 1 >= PATHFIND_LIST_MAX ) {
      retval = MERROR_OVERFLOW;
   } else {
      memcpy( &(list[*p_list_sz]), node, sizeof( struct PATHFIND_NODE ) );
      (*p_list_sz)++;
   }

   return retval;
}

static void pathfind_list_remove(
   struct PATHFIND_NODE* list, size_t* p_list_sz, size_t idx
) {
   assert( idx < *p_list_sz );
   while( idx + 1 < *p_list_sz ) {
      memcpy(
         &(list[idx]), &(list[idx + 1]), sizeof( struct PATHFIND_NODE ) );
      idx++;
   }
   (*p_list_sz)--;
}

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
   struct PATHFIND_NODE* p_adjacent, uint8_t dir,
   uint8_t iter_closed_idx,
   uint8_t tgt_x, uint8_t tgt_y,
   struct PATHFIND_NODE* open, size_t* p_open_sz,
   struct PATHFIND_NODE* closed, uint8_t closed_sz, uint8_t flags,
   struct DSEKAI_STATE* state
) {
   uint8_t i = 0,
      child_open_idx = 0;
   int16_t a_x = p_adjacent->coords.x,
      a_y = p_adjacent->coords.y;
   MERROR_RETVAL retval = MERROR_OK;

   pathfind_trace_printf( 1, "> testing adjacent tile at %d, %d...",
      p_adjacent->coords.x, p_adjacent->coords.y );
   
   /* Make sure adjacent is not in the closed list. */
   for( i = 0 ; closed_sz > i ; i++ ) {
      if( pathfind_cmp_eq( p_adjacent, &(closed[i]) ) ) {
         pathfind_trace_printf( 1, ">> tile is already closed!" );
         return PATHFIND_ERROR_CLOSED;
      }
   }

   /* Don't use occupied tiles. */
   if(
      (
         PATHFIND_FLAGS_TGT_OCCUPIED != (PATHFIND_FLAGS_TGT_OCCUPIED & flags) ||
         /* Target tile is occupied (e.g. we're pathfinding to another mobile),
          * so skip this check for it. */
         (tgt_x != p_adjacent->coords.x && tgt_y != p_adjacent->coords.y)
      ) &&
      /* Otherwise check for occupied tile. */
      /* TODO: Make pathfind_test_dir() not need t */
      0 > pathfind_test_dir(
         closed[iter_closed_idx].coords.x,
         closed[iter_closed_idx].coords.y, dir, state,
         state->tilemap ) 
   ) {
      pathfind_trace_printf( 1,
         ">> tile %d, %d blocked by mobile or terrain!",
         closed[iter_closed_idx].coords.x + gc_mobile_x_offsets[dir],
         closed[iter_closed_idx].coords.y + gc_mobile_y_offsets[dir] );
      return MOBILE_ERROR_BLOCKED;
   }

   /* Calculate adjacent pathfinding properties. */

   pathfind_trace_printf( 1, ">> tile parent %d, %d, distance %d",
      closed[iter_closed_idx].coords.x,
      closed[iter_closed_idx].coords.y,
      closed[iter_closed_idx].g );

   p_adjacent->g = closed[iter_closed_idx].g + 1;
   pathfind_trace_printf( 1, ">> tile distance is %d", p_adjacent->g );
   
   /* Use Manhattan heuristic since we can only move in 4 dirs. */
   p_adjacent->h = abs( a_x - tgt_x ) + abs( a_y - tgt_y );
   pathfind_trace_printf( 1, ">> tile heuristic is %d", p_adjacent->h );

   p_adjacent->f = p_adjacent->g + p_adjacent->h;
   pathfind_trace_printf( 1, ">> tile cost is %d", p_adjacent->f );

   for( i = 0 ; *p_open_sz > i ; i++ ) {
      /* Make sure adjacent is not already on the open list. */
      if( pathfind_cmp_eq( p_adjacent, &(open[i]) ) ) {
         return PATHFIND_ERROR_FARTHER;
      }
   }

   /* Add the child to the list. */
   pathfind_trace_printf( 1, ">> adding tile to open list" );
   retval = pathfind_list_append( open, p_open_sz, p_adjacent );
   if( MERROR_OK != retval ) {
      pathfind_trace_printf( 1, ">> open list full!" );
      return MERROR_OVERFLOW;
   }

   return child_open_idx;
}

#ifdef PATHFIND_TRACE

static void pathfind_dump_map(
   uint8_t x_max, uint8_t y_max,
   struct PATHFIND_NODE* open, uint8_t open_sz,
   struct PATHFIND_NODE* closed, uint8_t closed_sz
) {
   uint8_t x = 0,
      y = 0,
      i = 0;
   char map_char = ' ';

   printf( "---PATHFIND TILES---\n " );

   for( x = 0 ; x_max > x ; x++ ) {
      printf( "|%d", x );
   }
   printf( "\n" );

   for( y = 0 ; y_max > y ; y++ ) {
      printf( "%d", y );
      for( x = 0 ; x_max > x ; x++ ) {
         map_char = ' ';

         /* See if tile is open. */
         for( i = 0 ; open_sz > i ; i++ ) {
            if( open[i].coords.x == x && open[i].coords.y == y ) {
               map_char = '?';
            }
         }
         for( i = 0 ; closed_sz > i ; i++ ) {
            if( closed[i].coords.x == x && closed[i].coords.y == y ) {
               map_char = 'x';
            }
         }

         printf( "|%c", map_char );
      }
      printf( "\n" );
   }
}

#endif /* PATHFIND_TRACE */

int8_t pathfind_start(
   struct MOBILE* mover, uint8_t tgt_x, uint8_t tgt_y, uint8_t steps,
   uint8_t flags, struct DSEKAI_STATE* state
) {
   struct PATHFIND_NODE open[PATHFIND_LIST_MAX];
   struct PATHFIND_NODE closed[PATHFIND_LIST_MAX];
   struct PATHFIND_NODE adjacent;
   size_t open_sz = 0;
   uint8_t iter_idx = 0,
      closed_sz = 0,
      tgt_reached = 0,
      i = 0;
   MERROR_RETVAL retval = MOBILE_ERROR_BLOCKED;

   /* Zero out lists and nodes. */
   maug_mzero(
      open, sizeof( struct PATHFIND_NODE ) * PATHFIND_LIST_MAX );
   maug_mzero(
      closed, sizeof( struct PATHFIND_NODE ) * PATHFIND_LIST_MAX );
   maug_mzero( &adjacent, sizeof( struct PATHFIND_NODE ) );
   
   /* Add the start node to the open list. */
   adjacent.coords.x = mover->coords[1].x;
   adjacent.coords.y = mover->coords[1].y;
   pathfind_list_append( open, &open_sz, &adjacent );
   
   pathfind_trace_printf( 1, "---BEGIN PATHFIND---" );
   pathfind_trace_printf( 1, "pathfinding to %d, %d...", tgt_x, tgt_y );

   while( 0 < open_sz ) {
      iter_idx = pathfind_list_lowest_f( open, open_sz );

      /* Move the iter node to the closed list. */
      pathfind_trace_printf( 1,
         "moving %d, %d to closed list idx %d and evaluating...",
         open[iter_idx].coords.x, open[iter_idx].coords.y, closed_sz );
      memcpy(
         &(closed[closed_sz]), &(open[iter_idx]), 
         sizeof( struct PATHFIND_NODE ) );
      pathfind_list_remove( open, &open_sz, iter_idx );
      iter_idx = closed_sz;
      closed_sz++;
      
      /* Validate closed list size. */
      if( closed_sz >= PATHFIND_LIST_MAX ) {
         pathfind_trace_printf( 1, "> pathfind stack exceeded" );
         retval = MERROR_OVERFLOW; 
         goto cleanup;
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

      /* Limit pathfinding steps to what was requested. */
      if( closed_sz >= steps ) {
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
            &adjacent, i, iter_idx, tgt_x, tgt_y, open, &open_sz,
            closed, closed_sz, flags, state );

         pathfind_trace_printf( 1, ">> open list now has %d tiles", open_sz );

#ifdef PATHFIND_TRACE
         pathfind_dump_map( 10, 10, open, open_sz, closed, closed_sz );
#endif /* PATHFIND_TRACE */
      }
   }

   if( tgt_reached ) {
      /* Return the next closest tile to the target. */
      pathfind_trace_printf( 1, "> tgt reached! %d, %d",
         closed[1].coords.x, closed[1].coords.y );
      retval = closed[1].dir;
   } else {
      pathfind_trace_printf( 1, "> blocked! (closed sz %d)", closed_sz );
   }

cleanup:

   pathfind_trace_printf( 1, "---END PATHFIND---" );

   return retval;
}

