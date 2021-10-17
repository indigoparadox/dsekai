
#ifndef MOBILE_H
#define MOBILE_H

/*! \file mobile.h
 *  \brief Structs, functions and macros pertaining to interactive objects.
 */

/*! \brief A moving/interactive object in the world. */
struct PACKED MOBILE {
   uint8_t facing;
   uint8_t active;
   uint8_t hp;
   uint8_t mp;
   RESOURCE_ID sprite;
   /*! \brief Current tile on which object is located. */
   struct TILEMAP_COORDS coords;
   /*! \brief Previous tile, if object is currently moving. */
   struct TILEMAP_COORDS coords_prev;
   /*! \brief Number of steps remaining on X axis in current walk animation. */
   uint8_t steps_x;
   /*! \brief Number of steps remaining on Y axis in current walk animation. */
   uint8_t steps_y;
   struct ITEM* inventory;
};

/* TODO: Condense these into flags. */
#define MOBILE_FACING_SOUTH   0
#define MOBILE_FACING_NORTH   1
#define MOBILE_FACING_EAST    2
#define MOBILE_FACING_WEST    3

#define MOBILE_TYPE_PLAYER    1
#define MOBILE_TYPE_PRINCESS  2
#define MOBILE_TYPE_FOUNTAIN  3
#define MOBILE_TYPE_RASPTOR   4

#define MOBILE_IACT_NONE      0
#define MOBILE_IACT_TALK      1
#define MOBILE_IACT_ATTACK    2
#define MOBILE_IACT_SHOP      3

#define MOBILE_STEP_NOP       0
#define MOBILE_STEP_WAIT_TALK 1
#define MOBILE_STEP_TALK      2

uint8_t mobile_walk_start( struct MOBILE*, int8_t, int8_t );
void mobile_interact( struct MOBILE*, struct MOBILE* );
void mobile_interact_txy( struct MOBILE*, struct MOBILE*, uint16_t, uint16_t );
void mobile_state_animate( struct DSEKAI_STATE* );
void mobile_animate( struct MOBILE*, struct TILEMAP* );
void mobile_draw(
   const struct MOBILE*, const struct DSEKAI_STATE*, int16_t, int16_t );
void mobile_deinit( struct MOBILE* );

#endif /* MOBILE_H */

