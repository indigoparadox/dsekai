
#ifndef ENGINES_H
#define ENGINES_H

/**
 * \addtogroup dsekai_engines DSekai Engines
 * \brief Central state and subsystem coordination.
 *
 * \{
 */

/*! \file engines.h
 *  \brief Macros, structs, and prototypes for the various engine types.
 */

/**
 * \addtogroup dsekai_engines_specific_struct DSekai Engine-Specific Struct
 * \brief Structs containing information only used by certain engines.
 *
 * These are kept in a separate handle from the main ::DSEKAI_STATE, attached
 * to DSEKAI_STATE::engine_state_handle. This handle is cleared and
 * reallocated for the new engine when the engine changes.
 *
 * \{
 */

#define ENGINE_TABLE( f ) f( 0, NONE, title ) f( 1, TOPDOWN, topdown )

/*! \brief Display the title screen. */
#define ENGINE_TYPE_NONE 0
/*! \brief Use the topdown 2D engine. */
#define ENGINE_TYPE_TOPDOWN 1
/*! \brief Use the POV 3D engine. */
#define ENGINE_TYPE_POV 2

/**
 * \brief State for ::ENGINE_TYPE_NONE. A simple title screen engine.
 */
struct TITLE_STATE {
   /*! \brief Index of the currently highlighted option. */
   uint8_t option_high;
};

/**
 * \brief State for ::ENGINE_TYPE_TOPDOWN. Displays a top-down 2-dimensional
 *        view of the world.
 */
struct TOPDOWN_STATE {
   /*! \brief Left horizontal offset of the current viewport in pixels. */
   int16_t screen_scroll_x;
   /*! \brief Top vertical offset of the current viewport in pixels. */
   int16_t screen_scroll_y;
   /*! \brief Left horizontal offset of the current viewport in tiles. */
   int16_t screen_scroll_tx;
   /*! \brief Top vertical offset of the current viewport in tiles. */
   int16_t screen_scroll_ty;
   /*! \brief Horizontal offset the viewport is scrolling to in pixels. */
   int16_t screen_scroll_x_tgt;
   /*! \brief Vertical offset the viewport is scrolling to in pixels. */
   int16_t screen_scroll_y_tgt;
};

/**
 * \brief State for ::ENGINE_TYPE_POV. Displays a first-person 3D view of the
 *        world using raycasting.
 */
struct POV_STATE {
   double plane_y;
};

/*! \} */

#define DSEKAI_FLAG_INPUT_BLOCKED 0x01

/*! \brief General/shared state of the running engine in memory. */
struct DSEKAI_STATE {
   /**
    * \brief Array consisting of all items in the current game world.
    *
    * Items in this array are considered to "exist" when the first character
    * of ITEM::name is not NULL or '\0'. Items with no name may be replaced by
    * new items.
    */
   struct ITEM items[DSEKAI_ITEMS_MAX];

   struct TILEMAP map;

   /**
    * \brief Array of currently loaded MOBILE objects on this map.
    *
    * Mobiles in this array are considered to "exist" when
    * ::MOBILE_FLAG_ACTIVE is enabled in their MOBILE::flags.
    */
   struct MOBILE mobiles[DSEKAI_MOBILES_MAX];
   /*! \brief Currently active player MOBILE. Stays between maps. */
   struct MOBILE player;

   /*!
    * \brief Contains the currently loaded \ref dsekai_engines_specific_struct.
    */
   MEMORY_HANDLE engine_state_handle;

   uint8_t input_blocked_countdown;

   /*! \brief The number of loops until DSEKAI_STATE::ani_sprite_countdown
    *         changes.
    */
   uint8_t ani_sprite_countdown;
   /*! \brief The horizontal offset of all on-screen ::MOBILE sprites in pixels.
    */
   uint16_t ani_sprite_x;

   /*! \brief Contains ::WINDOW structs currently on-screen. */
   MEMORY_HANDLE windows_handle;
   /*! \brief Number of active ::WINDOW structs in DSEKAI_STATE::windows_handle.
    */
   uint16_t windows_count;

   /*! \brief When holding a valid string, triggers the map change process. */
   char warp_to[TILEMAP_NAME_MAX];
   /*! \brief Sets the player MOBILE::coords horizontal on map change. */
   uint8_t warp_to_x;
   /*! \brief Sets the player MOBILE::coords vertical on map change. */
   uint8_t warp_to_y;

   uint16_t engine_state;

   uint8_t engine_sel;

   uint8_t flags;
};

/**
 * \brief Do generic mobile animation and execute their scripts.
 * \param state Locked ::MEMORY_PTR for current ::DSEKAI_STATE.
 */
void engines_animate_mobiles( struct DSEKAI_STATE* state );

/**
 * \brief Central loop iteration handler. Calls engine-specific callbacks.
 * \param state_handle Unlocked ::MEMORY_HANDLE for current ::DSEKAI_STATE.
 * \return 1 if engine should continue executing or 0 if it should quit.
 */
int16_t engines_loop_iter( MEMORY_HANDLE state_handle );

/**
 * \brief Handle generic player movement commmand.
 * \param dir_move
 * \param state Locked ::MEMORY_PTR for current ::DSEKAI_STATE.
 */
int16_t engines_handle_movement( int8_t dir_move, struct DSEKAI_STATE* state );

/*! \} */

typedef int16_t (*ENGINES_SETUP)( struct DSEKAI_STATE* state );
typedef int16_t (*ENGINES_INPUT)( char in_char, struct DSEKAI_STATE* state );
typedef void (*ENGINES_ANIMATE)( struct DSEKAI_STATE* state );
typedef void (*ENGINES_DRAW)( struct DSEKAI_STATE* state );

#define ENGINES_SETUP_PROTOTYPES( idx, eng, prefix ) int16_t prefix ## _setup( struct DSEKAI_STATE* state );

ENGINE_TABLE( ENGINES_SETUP_PROTOTYPES )

#define ENGINES_INPUT_PROTOTYPES( idx, eng, prefix ) int16_t prefix ## _input( char in_char, struct DSEKAI_STATE* state );

ENGINE_TABLE( ENGINES_INPUT_PROTOTYPES )

#define ENGINES_ANIMATE_PROTOTYPES( idx, eng, prefix ) void prefix ## _animate( struct DSEKAI_STATE* state );

ENGINE_TABLE( ENGINES_ANIMATE_PROTOTYPES )

#define ENGINES_DRAW_PROTOTYPES( idx, eng, prefix ) void prefix ## _draw( struct DSEKAI_STATE* state );

ENGINE_TABLE( ENGINES_DRAW_PROTOTYPES )

#ifdef ENGINES_C

#define ENGINES_LIST_SETUP( idx, eng, prefix ) prefix ## _setup,

const ENGINES_SETUP gc_engines_setup[] = {
   ENGINE_TABLE( ENGINES_LIST_SETUP )
};

#define ENGINES_LIST_INPUT( idx, eng, prefix ) prefix ## _input,

const ENGINES_INPUT gc_engines_input[] = {
   ENGINE_TABLE( ENGINES_LIST_INPUT )
};

#define ENGINES_LIST_ANIMATE( idx, eng, prefix ) prefix ## _animate,

const ENGINES_ANIMATE gc_engines_animate[] = {
   ENGINE_TABLE( ENGINES_LIST_ANIMATE )
};

#define ENGINES_LIST_DRAW( idx, eng, prefix ) prefix ## _draw,

const ENGINES_DRAW gc_engines_draw[] = {
   ENGINE_TABLE( ENGINES_LIST_DRAW )
};

#else

#endif /* ENGINES_C */

#endif /* ENGINES_H */

