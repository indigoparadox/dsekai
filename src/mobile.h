
#ifndef MOBILE_H
#define MOBILE_H

/**
 * \addtogroup dsekai_mobiles Mobile Objects
 * \brief Player- or script-controlled world objects or "mobiles".
 *
 * \{
 */

/*! \file mobile.h
 *  \brief Structs, functions and macros pertaining to interactive objects.
 */

/**
 * \addtogroup dsekai_mobiles_spritesheets_sect Mobile Spritesheets
 * \{
 * \page dsekai_mobiles_spritesheets
 * 
 * Spritesheets for mobiles are divided up as follows:
 *
 * |      | 16px                       | 16px
 * |------|----------------------------|----------------------------
 * | 16px | ::MOBILE_DIR_SOUTH Frame 1 | ::MOBILE_DIR_SOUTH Frame 2
 * | 16px | ::MOBILE_DIR_NORTH Frame 1 | ::MOBILE_DIR_NORTH Frame 2
 * | 16px | ::MOBILE_DIR_EAST Frame 1  | ::MOBILE_DIR_EAST Frame 2
 * | 16px | ::MOBILE_DIR_WEST Frame 1  | ::MOBILE_DIR_WEST Frame 2
 *
 * \}
 */

/**
 * \addtogroup dsekai_mobiles_mp_hp_sect Mobile MP/HP
 * \brief Details of how the MOBILE::mp_hp field works.
 * \{
 *
 * \page dsekai_mobiles_mp_hp
 * \{
 *
 * \section dsekai_mobiles_mp_hp_access Access and Modification
 * These fields are combined to save space in an expected 16-bit struct
 * alignment.
 *
 * Use mobile_incr_hp() to modify.
 *
 * \section dsekai_mobiles_mp_hp_death Death Handling
 * Death occurs when the HP half reaches 0.
 *
 * This value should be set to ::MOBILE_HP_DEATH on this mobile's death
 * blow. From there, the engine will increment it up to its final value,
 * -1, where the mobile will be deleted.
 *
 * This allows for a relatively simple "blinking" death animation to occur
 * by hiding frames for mobiles on negative even HP values.
 * \}
 */

/**
 * \brief Bitmask denoting half of MOBILE::mp_hp devoted to MP.
 */
#define MOBILE_HP_MASK 0x00ff

/**
 * \brief Bitmask denoting half of MOBILE::mp_hp devoted to HP.
 */
#define MOBILE_MP_MASK 0xff00

/**
 * \brief Value to set MOBILE::hp to on death but before deactivation.
 */
#define MOBILE_HP_DEATH -10

/**
 * \relates MOBILE
 * \brief Increment just the HP half of MOBILE::mp_hp.
 */
#define mobile_incr_hp( m, v ) (m)->mp_hp = (((m)->mp_hp & MOBILE_MP_MASK) | (((((m)->mp_hp & MOBILE_HP_MASK) + (v)) & MOBILE_HP_MASK) & MOBILE_HP_MASK))

/*! \} */

/**
 * \relates MOBILE
 * \brief Get the \ref dsekai_mobiles_directions the mobile is facing.
 */
#define mobile_get_dir( m ) ((m)->flags & MOBILE_DIR_MASK)

/**
 * \relates MOBILE
 * \brief Set ::MOBILE_DIR_MASK in MOBILE::flags to the specified
 *        \ref dsekai_mobiles_directions.
 * \param m Locked ::MEMORY_PTR to ::MOBILE to modify.
 * \param v \ref dsekai_mobiles_directions to set the mobile facing.
 */
#define mobile_set_dir( m, v ) (m)->flags = (((m)->flags & ~MOBILE_DIR_MASK) | ((v) & MOBILE_DIR_MASK))

/**
 * \relates MOBILE
 * \brief Get the current interaction count of the specified ::MOBILE.
 * \param m Locked ::MEMORY_PTR to ::MOBILE to query.
 */
#define mobile_get_icount( m ) (((m)->flags & MOBILE_ICOUNT_MASK) >> 12)

/**
 * \relates MOBILE
 * \brief Increment ::MOBILE_ICOUNT_MASK by a given value.
 * \param m Locked ::MEMORY_PTR to ::MOBILE to modify.
 * \param v Value to increment counter by.
 */
#define mobile_incr_icount( m, v ) (m)->flags = (((m)->flags & ~MOBILE_ICOUNT_MASK) | ((mobile_get_icount( m ) + (((v) << 12) & MOBILE_ICOUNT_MASK)) & MOBILE_ICOUNT_MASK))

/**
 * \addtogroup dsekai_mobile_flags Mobile Object Flags
 * \brief Flags controlling ::MOBILE object behavior.
 * \{
 */

/**
 * \brief Bitmask defining bits used to indicate the number of times this
 *        ::MOBILE has been interacted with, for ::SCRIPT purposes.
 */
#define MOBILE_ICOUNT_MASK 0xf000

/**
 * \brief Bitmask defining bits used to indicate which of the
 *        \ref dsekai_mobiles_directions a mobile is currently facing.
 */
#define MOBILE_DIR_MASK 0x0007

/**
 * \addtogroup dsekai_mobile_flags_type Mobile Object Type Flags
 * \brief Types instructing the engine how to handle a ::MOBILE.
 * \{
 */

/**
 * \brief MOBILE::flags indicating single-map lifecycle for this mobile.
 */
#define MOBILE_TYPE_NORMAL 0x0000

/**
 * \brief MOBILE::flags indicating that this mobile persists between tilemaps.
 */
#define MOBILE_TYPE_SPECIAL   0x0008

/*! \brief Reserved for future use. */
#define MOBILE_TYPE_RES1 0x0010

/*! \brief Reserved for future use. */
#define MOBILE_TYPE_RES2 0x0018

/*! \} */

/**
 * \brief Bitmask defining bits used to indicate \ref dsekai_mobile_flags_type.
 */
#define MOBILE_TYPE_MASK 0x0018

/**
 * \brief MOBILE::flags indicating that this mobile is extant and active.
 */
#define MOBILE_FLAG_ACTIVE 0x0100

/**
 * \brief MOBILE::flags indicating that this is a player ::MOBILE.
 */
#define MOBILE_FLAG_PLAYER 0x0200

/*! \} */

/**
 * \related MOBILE
 * \brief MOBILE::map_gid value indicating mobile is present on all tilemaps.
 *
 * This is generally only used for the player.
 */
#define MOBILE_MAP_GID_ALL 65535

#define mobile_get_sprite( m ) ((m)->sprite_id)

/* TODO: Combine steps_x and steps_y? */

/*! \brief A moving/interactive object in the world. */
struct MOBILE {
   char* name;
   /**
    * \brief \ref dsekai_mobile_flags affecting this mobile's display and
    *        behavior.
    */
   uint16_t flags;
   /**
    * \brief TILEMAP_SPAWN::gid of the spawner that spawned this mobile.
    */
   uint16_t spawner_gid;
   /**
    * \brief TILEMAP::gid of the tilemap this mobile was spawned on.
    */
   uint16_t map_gid;
   /**
    * \brief The combined \ref dsekai_mobiles_mp_hp field.
    */
   int16_t mp_hp;
   /**
    * \brief Current on-screen pixel left of sprite. -1 if off-screen.
    *
    * This may vary, depending on the engine type, but is also used to place
    * animations over the mobile.
    */
   int16_t screen_px;
   /**
    * \brief Current on-screen pixel left of sprite. -1 if off-screen.
    *
    * This may vary, depending on the engine type, but is also used to place
    * animations over the mobile.
    */
   int16_t screen_py;
   /**
    * \brief Index of the mobile's \ref dsekai_mobiles_spritesheets_sect
    *        loaded in the \ref unilayer_graphics_cache.
    */
   int16_t sprite_id;
   /*! \brief Current tile on which this mobile is located. */
   struct TILEMAP_COORDS coords;
   /*! \brief Previous tile, if this mobile is currently moving. */
   struct TILEMAP_COORDS coords_prev;
   /**
    * \brief Number of steps remaining in current walk animation.
    * \deprecated To be combined with MOBILE::steps_y.
    */
   uint8_t steps_x;
   /**
    * \brief Number of steps remaining in current walk animation.
    * \deprecated To be combined with MOBILE::steps_x.
    */
   uint8_t steps_y;
   /*! \brief Index currently executing behavior script in TILEMAP::scripts. */
   int16_t script_id;
   /*! \brief Position in currently executing behavior script. */
   int16_t script_pc;
   /*! \brief Local stack used to store state for this mobile's ::SCRIPT.
    *
    *  This should only be manipulated by mobile_stack_push() and
    *  mobile_stack_pop().
    */
   int8_t script_stack[SCRIPT_STACK_DEPTH];
   /*! \brief Delay script for this many frames. */
   uint16_t script_wait_frames;
};

/**
 * \addtogroup dsekai_mobiles_directions Cardinal Directions
 * \brief Directions in which ::MOBILE objects may face.
 *
 * \{
 */

/*! \brief MOBILE::dir indicating down direction for movement/animation. */
#define MOBILE_DIR_SOUTH   0
/*! \brief MOBILE::dir indicating up direction for movement/animation. */
#define MOBILE_DIR_NORTH   1
/*! \brief MOBILE::dir indicating right direction for movement/animation. */
#define MOBILE_DIR_EAST    2
/*! \brief MOBILE::dir indicating left direction for movement/animation. */
#define MOBILE_DIR_WEST    3

/*! \} */

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
 * \param state ::MEMORY_PTR to current engine ::DSEKAI_STATE.
 * \return ::MEMORY_PTR to the colliding mobile in ms.
 */
struct MOBILE* mobile_get_facing(
   struct MOBILE* m, struct TILEMAP* t, struct DSEKAI_STATE* state );

/**
 * \brief Force a ::MOBILE to jump to the SCRIPT_ACTION_INTERACT in its
 *        ::SCRIPT.
 * \param actor ::MEMORY_PTR to the MOBILE sending the interact call.
 * \param actee ::MEMORY_PTR to the MOBILE which should jump the interaction.
 * \param t ::MEMORY_PTR to the currently loaded TILEMAP.
 */
struct MOBILE* mobile_interact(
   struct MOBILE* actor, struct MOBILE* actee, struct TILEMAP* t
) SECTION_MOBILE;

/**
 * \brief Map ::MOBILE animation frames to a number of real frames elapsed.
 * \param state Locked ::MEMORY_PTR to current engine ::DSEKAI_STATE.
 */
void mobile_state_animate( struct DSEKAI_STATE* state );

/**
 * \brief Perform animation frame for the given MOBILE.
 * \param m ::MEMORY_PTR to MOBILE to animate.
 * \param state Locked ::MEMORY_PTR to current engine ::DSEKAI_STATE.
 */
void mobile_animate( struct MOBILE* m, struct DSEKAI_STATE* state );

/**
 * \brief Prepare a ::MOBILE for deallocation.
 * \param t ::MEMORY_PTR to a ::MOBILE to deinitialize.
 */
void mobile_deinit( struct MOBILE* ) SECTION_MOBILE;

/**
 * \brief Push a value onto MOBILE::script_stack.
 * \param m ::MEMORY_PTR to ::MOBILE on which to push to ::MOBILE::script_stack.
 * \param v Value to push onto the stack.
 */
void mobile_stack_push( struct MOBILE* m, int8_t v ) SECTION_MOBILE;

/**
 * \brief Pop a value from a MOBILE::script_stack.
 * \param m ::MEMORY_PTR to ::MOBILE from which to pop ::MOBILE::script_stack.
 * \return The value popped.
 */
int8_t mobile_stack_pop( struct MOBILE* m ) SECTION_MOBILE;

/**
 * \brief Execute the next available ::SCRIPT_STEP in the currently running
 *        ::SCRIPT on a MOBILE.
 * \param m ::MEMORY_PTR to the MOBILE running the desired ::SCRIPT.
 * \param state Locked ::MEMORY_PTR to current engine ::DSEKAI_STATE.
 */
void mobile_execute( struct MOBILE* m, struct DSEKAI_STATE* state );

/**
 * \brief Allocate a mobile or select player mobile slot and initialize it
 *        with configuration generic to ALL mobiles.
 * \param state Locked ::MEMORY_PTR to current engine ::DSEKAI_STATE.
 * \param flags \ref dsekai_mobile_flags applying to the spawned ::MOBILE.
 */
struct MOBILE* mobile_spawn_single(
   uint16_t flags, struct DSEKAI_STATE* state ) SECTION_MOBILE;

/**
 * \brief Spawn from ::TILEMAP::spawners according to spawner rules.
 * \param state Locked ::MEMORY_PTR to the current engine ::DSEKAI_STATE.
 * \param t Locked ::MEMORY_PTR to ::TILEMAP on which to execute spawners.
 */
void mobile_spawns(
   struct TILEMAP* t, struct DSEKAI_STATE* state ) SECTION_MOBILE;

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
/*! \brief Lookup table for vertical offset based on mobile_get_dir(). */
extern const int8_t gc_mobile_y_offsets[4];
/*! \brief Lookup table for horizontal offset based on mobile_get_dir(). */
extern const int8_t gc_mobile_x_offsets[4];
#endif /* MOBILE_C */

/*! \} */

#endif /* MOBILE_H */

