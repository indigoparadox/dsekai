
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
   /*! \brief MEMORY_PTR to currently executing behavior script. */
   struct SCRIPT_STEP* script;
   /*! \brief Position in currently executing behavior script. */
   uint16_t script_pc;
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

/**
 * \brief Have the given MOBILE attempt to begin walking movement/animation.
 * \param m MEMORY_PTR to MOBILE that should attempt moving.
 * \param x Tile offset to attempt moving horizontally.
 * \param y Tile offset to attempt moving vertically.
 *
 * This may fail if the mobile is already walking, or is blocked from moving
 * in that direction by a map obstacle or other mobile.
 */
uint8_t mobile_walk_start( struct MOBILE* m, int8_t x, int8_t y );

void mobile_interact( struct MOBILE*, struct MOBILE* );
void mobile_interact_txy( struct MOBILE*, struct MOBILE*, uint16_t, uint16_t );
void mobile_state_animate( struct DSEKAI_STATE* );

/**
 * \brief Perform animation frame for the given MOBILE.
 * \param m MEMORY_PTR to MOBILE to animate.
 * \param t MEMORY_PTR to TILEMAP on which MOBILE is located.
 */
void mobile_animate( struct MOBILE* m, struct TILEMAP* t );

/**
 * \brief Draw the given MOBILE in its current animation frame.
 * \param m MEMORY_PTR to MOBILE to draw on screen.
 * \param state MEMORY_PTR to engine state to use for animation.
 * \param screen_x Horizontal screen pixel offset to draw mobile at.
 * \param screen_y Vertical screen pixel offset to draw mobile at.
 */
void mobile_draw(
   const struct MOBILE* m, const struct DSEKAI_STATE* state,
   int16_t screen_x, int16_t screen_y );
void mobile_deinit( struct MOBILE* );

#ifdef MOBILE_C
/*! \brief Lookup table for next walking offset based on current offset. */
const int8_t gc_mobile_step_table_normal_pos[16] = {
   0, 0, 0, 0,       /*  0,  1,  2,  3 */
   3, 3, 3, 3,       /*  4,  5,  6,  7 */
   7, 7, 7, 7,       /*  8,  9, 10, 11 */
   11, 11, 11, 11    /* 12, 13, 14, 15 */
};
#else
extern const int8_t gc_mobile_step_table_normal_pos[16];
#endif /* MOBILE_C */

#endif /* MOBILE_H */

