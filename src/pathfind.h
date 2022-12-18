
#ifndef PATHFIND_H
#define PATHFIND_H

/**
 * \addtogroup dsekai_pathfind Pathfinding
 * \{
 */

/**
 * \file pathfind.h
 */

struct PATHFIND_NODE {
   struct TILEMAP_COORDS coords;
   /*! \brief Total node cost. */
   uint8_t f;
   /*! \brief Distance from node to pathfinding start. */
   uint8_t g; 
   /*! \brief Estimated distance from node to pathfinding target. */
   uint8_t h;
   /*! \brief Direction of this node from its parent. */
   int8_t dir;
};

#define PATHFIND_ERROR_FARTHER -2

#define PATHFIND_ERROR_CLOSED -3

#ifdef PATHFIND_TRACE
#  define pathfind_trace_printf( lvl, ... ) debug_printf( lvl, __VA_ARGS__ )
#else
/**
 * \brief Alias for debug_printf() enabled when PATHFIND_TRACE is defined.
 *
 * This was made its own alias since pathfinding can get rather chatty when it's
 * enabled!
 */
#  define pathfind_trace_printf( ... )
#endif /* PATHFIND_TRACE */

#define pathfind_cmp_eq( a, b ) (((a)->coords.x == (b)->coords.x) && (((a)->coords.y == (b)->coords.y)))

int8_t pathfind_start(
   struct MOBILE* mover, uint8_t tgt_x, uint8_t tgt_y, uint8_t steps,
   struct DSEKAI_STATE* state, struct TILEMAP* t );

/**
 * \brief Check for blocking terrain or other ::MOBILE objects preventing
 *        movement in dir_move.
 * \param mover Locked ::MEMORY_PTR to ::MOBILE performing the movement.
 * \param dir_move \ref dsekai_mobiles_directions to attempt to move in.
 * \param state Locked ::MEMORY_PTR for current ::DSEKAI_STATE.
 * \param force 0 if movement should not be attempted if input is blocked
 *              globally, or 1 if it should.
 * \param t Locked ::MEMORY_PTR for DSEKAI_STATE::map_handle.
 * \return 1 if successful,
 *         or ::MOBILE_ERROR_BLOCKED if movement is blocked.
 * \todo TODO: Determine terrain movement speed.
 */
#define pathfind_test_dir( x, y, dir_move, state, t ) (TILEMAP_ERROR_BLOCKED != tilemap_collide( x, y, dir_move, t ) && (NULL == mobile_get_facing( x, y, dir_move, t, state )) ? 1 : MOBILE_ERROR_BLOCKED)

/*! \} */ /* dsekai_pathfind */

#endif /* !PATHFIND_H */

