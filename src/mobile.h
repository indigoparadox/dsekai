
#ifndef MOBILE_H
#define MOBILE_H

/*! \file mobile.h
 *  \brief Structs, functions and macros pertaining to interactive objects.
 */

/*! \brief A moving/interactive object in the world. */
struct PACKED MOBILE {
   /*! \brief The direction the mobile is currently facing. */
   uint8_t dir;
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
   /*! \brief Index currently executing behavior script in TILEMAP::scripts. */
   int16_t script_id;
   /*! \brief Position in currently executing behavior script. */
   int16_t script_pc;
   /*! \brief Position to return to after a jump. */
   int16_t script_pc_prev;
   /*! \brief Delay script until this result from graphics_get_ms(). */
   uint32_t script_next_ms;
};

/*! \brief MOBILE::dir indicating down direction for movement/animation. */
#define MOBILE_DIR_SOUTH   0
/*! \brief MOBILE::dir indicating up direction for movement/animation. */
#define MOBILE_DIR_NORTH   1
/*! \brief MOBILE::dir indicating right direction for movement/animation. */
#define MOBILE_DIR_EAST    2
/*! \brief MOBILE::dir indicating left direction for movement/animation. */
#define MOBILE_DIR_WEST    3

/**
 * \brief Have the given MOBILE attempt to begin walking movement/animation.
 * \param m ::MEMORY_PTR to MOBILE that should attempt moving.
 * \param dir Index of direction offsets in ::gc_mobile_x_offsets.
 *
 * This may fail if the mobile is already walking, or is blocked from moving
 * in that direction by a map obstacle or other mobile.
 */
uint8_t mobile_walk_start( struct MOBILE* m, uint8_t dir );

/**
 * \brief Get a ::MEMORY_PTR to the mobile m is currently facing.
 * \param m MOBILE to center the check around.
 * \param ms Array of MOBILE structs currently on the map.
 * \param ms_sz Size of array ms.
 * \return ::MEMORY_PTR to the colliding mobile in ms.
 */
struct MOBILE* mobile_get_facing(
   struct MOBILE* m, struct MOBILE ms[], int ms_sz );

/**
 * \brief Force a ::MOBILE to jump to the SCRIPT_ACTION_INTERACT in its
 *        ::SCRIPT.
 * \param actor ::MEMORY_PTR to the MOBILE sending the interact call.
 * \param actee ::MEMORY_PTR to the MOBILE which should jump the interaction.
 * \param t ::MEMORY_PTR to the currently loaded TILEMAP.
 */
struct MOBILE* mobile_interact(
   struct MOBILE* actor, struct MOBILE* actee, struct TILEMAP* t );

/**
 * \brief Map ::MOBILE animation frames to a number of real frames elapsed.
 * \param state ::MEMORY_PTR to current engine ::DSEKAI_STATE.
 */
void mobile_state_animate( struct DSEKAI_STATE* state );

/**
 * \brief Perform animation frame for the given MOBILE.
 * \param m ::MEMORY_PTR to MOBILE to animate.
 * \param t ::MEMORY_PTR to TILEMAP on which MOBILE is located.
 */
void mobile_animate( struct MOBILE* m, struct TILEMAP* t );

/**
 * \brief Draw the given MOBILE in its current animation frame.
 * \param m ::MEMORY_PTR to MOBILE to draw on screen.
 * \param state ::MEMORY_PTR to engine state to use for animation.
 * \param screen_x Horizontal screen pixel offset to draw mobile at.
 * \param screen_y Vertical screen pixel offset to draw mobile at.
 */
void mobile_draw(
   struct MOBILE* m, const struct DSEKAI_STATE* state,
   int16_t screen_x, int16_t screen_y );

/**
 * \brief Prepare a ::MOBILE for deallocation.
 * \param t ::MEMORY_PTR to a ::MOBILE to deinitialize.
 */
void mobile_deinit( struct MOBILE* );

/**
 * \brief Get the MOBILE being faced by a given MOBILE if there is one.
 * \param m ::MEMORY_PTR to a MOBILE on which to center the search.
 * \param mobiles Array of MOBILE objects currently loaded.
 * \param mobiles_sz Size of mobiles array.
 * \return ::MEMORY_PTR to MOBILE being faced if one found, or NULL otherwise.
 */
struct MOBILE* mobile_get_dir(
   struct MOBILE* m, struct MOBILE mobiles[], int mobiles_sz );

/**
 * \brief Execute the next available ::SCRIPT_STEP in the currently running
 *        ::SCRIPT on a MOBILE.
 * \param m ::MEMORY_PTR to the MOBILE running the desired ::SCRIPT.
 * \param t Currently loaded ::TILEMAP.
 * \param state ::MEMORY_PTR to current engine ::DSEKAI_STATE.
 */
void mobile_execute(
   struct MOBILE* m, struct TILEMAP* t, struct DSEKAI_STATE* state );

#ifdef MOBILE_C
const int8_t gc_mobile_step_table_normal_pos[16] = {
   0, 0, 0, 0,       /*  0,  1,  2,  3 */
   3, 3, 3, 3,       /*  4,  5,  6,  7 */
   7, 7, 7, 7,       /*  8,  9, 10, 11 */
   11, 11, 11, 11    /* 12, 13, 14, 15 */
};

const int8_t gc_mobile_y_offsets[4] = {
   1, -1, 0, 0
};

const int8_t gc_mobile_x_offsets[4] = {
   0, 0, 1, -1
};
#else
/*! \brief Lookup table for next walking offset based on current offset. */
extern const int8_t gc_mobile_step_table_normal_pos[16];
/*! \brief Lookup table for vertical offset based on MOBILE::dir. */
extern const int8_t gc_mobile_y_offsets[4];
/*! \brief Lookup table for horizontal offset based on MOBILE::dir. */
extern const int8_t gc_mobile_x_offsets[4];
#endif /* MOBILE_C */

#endif /* MOBILE_H */

