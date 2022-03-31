
#ifndef ENGINES_H
#define ENGINES_H

/**
 * \addtogroup dsekai_engines Engines
 * \brief Central state and subsystem coordination.
 *
 * \{
 */

/*! \file engines.h
 *  \brief Macros, structs, and prototypes for the various engine types.
 */

#define MINIMAP_X (SCREEN_W - TILEMAP_TW - 10)
#define MINIMAP_Y (10)

/**
 * \addtogroup dsekai_engines_transitions Engine Transitions
 * \brief Flags for specifying screen transitions.

 * \{
 */

/**
 * \brief Mask indicating bits of DSEKAI_STATE::transition that denote the 
 *        current transition's frame.
 *
 * A frame of 0 indicates no transition is currently in progress.
 */
#define DSEKAI_TRANSITION_MASK_FRAME 0x0f

/**
 * \brief Mask indicating bits of DSEKAI_STATE::transition that denote the
 *        current transition's type.
 *
 * Only used if bits masked by ::DSEKAI_TRANSITION_MASK_FRAME are greather than
 * zero.
 */
#define DSEKAI_TRANSITION_MASK_TYPE 0x70

/**
 * \brief If this bit is on, then the currently playing transition should be
 *        opening. If it's off, the transition is closing.
 */
#define DSEKAI_TRANSITION_DIR_OPEN  0x80

#define DSEKAI_TRANSITION_TYPE_CURTAIN    0x00
#define DSEKAI_TRANSITION_TYPE_ZOOM       0x10

/*! \} */

#ifndef ENGINES_TOKENS_ONLY

/**
 * \addtogroup dsekai_engines_specific_struct Engine-Specific Struct
 * \brief Structs containing information only used by certain engines.
 *
 * These are kept in a separate handle from the main ::DSEKAI_STATE, attached
 * to DSEKAI_STATE::engine_state_handle. This handle is cleared and
 * reallocated for the new engine when the engine changes.
 *
 * \{
 */

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

#ifndef NO_ENGINE_POV

/**
 * \brief State for ::ENGINE_TYPE_POV. Displays a first-person 3D view of the
 *        world using raycasting.
 */
struct POV_STATE {
   uint8_t dirty;
   uint8_t minimap[TILEMAP_TH * TILEMAP_TW];
   uint16_t inc;
};

#endif /* !NO_ENGINE_POV */

#ifndef NO_ENGINE_EDITOR

/**
 * \addtoground dsekai_engines_editor Tilemap Editor
 * \brief In-world editor for tilemaps.
 */

/**
 * \relates EDITOR_STATE
 * \brief EDITOR_STATE::flags indicating editor is currently active.
 */
#define EDITOR_FLAG_ACTIVE       0x01

/**
 * \relates EDITOR_STATE
 * \brief EDITOR_STATE::flags indicating next frame should not be blocked out.
 *
 * This is a quality of life improvement so that it's easier to see while
 * cycling through tiles.
 */
#define EDITOR_FLAG_FORCE_FRAME  0x02

struct EDITOR_STATE {
   /*! \brief Tile coordinates currently being edited. */
   struct TILEMAP_COORDS coords;
   /*! \deprecated */
   uint8_t current_tile;
   /*! \deprecated */
   uint8_t selected_tile;
   /*! \brief Bitfield controlling behavior and flow of editor. */
   uint8_t flags;
};

/*! \} */

#endif /* !NO_ENGINE_EDITOR */

/*! \} */

/**
 * \relates DSEKAI_STATE
 * \brief DSEKAI_STATE::flags indicating no player input should be accepted.
 */
#define DSEKAI_FLAG_INPUT_BLOCKED   0x01

#define DSEKAI_FLAG_MENU_BLOCKED    0x02

/**
 * \relates DSEKAI_STATE
 * \brief DSEKAI_STATE::flags indicating every frame should be drawn fresh on
 *        a clean black canvas (unless the menu is open).
 */
#define DSEKAI_FLAG_BLANK_FRAME     0x04

/**
 * \relates DSEKAI_STATE
 * \brief DSEKAI_STATE::engine_state indicating engine is has not yet
 *        initialized its specific structures.
 */
#define ENGINE_STATE_OPENING 1

/**
 * \relates DSEKAI_STATE
 * \brief DSEKAI_STATE::engine_state indicating engine is running.
 */
#define ENGINE_STATE_RUNNING 2

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

   /*! \brief Engine type tilemap is supposed to be used with. */
   uint8_t engine_type;

   uint8_t engine_type_change;

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

   /*! \brief When holding a valid string, triggers the map change process. */
   char warp_to[TILEMAP_NAME_MAX];
   /*! \brief Sets the player MOBILE::coords horizontal on map change. */
   uint8_t warp_to_x;
   /*! \brief Sets the player MOBILE::coords vertical on map change. */
   uint8_t warp_to_y;

   /*! \brief Current engine state (see below in struct reference). */
   uint16_t engine_state;

   /*! \brief Global boolean values dictating engine state and behavior. */
   uint8_t flags;

   uint8_t transition;

#ifndef NO_ENGINE_EDITOR

   struct EDITOR_STATE editor;

#endif /* !NO_ENGINE_EDITOR */

   struct MENU_STATE menu;
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

void engines_exit_to_title( struct DSEKAI_STATE* state );

void engines_set_transition(
   struct DSEKAI_STATE* state, uint8_t trans_type, uint8_t trans_open );

void engines_draw_transition( struct DSEKAI_STATE* state );

#endif /* !ENGINES_TOKENS_ONLY */

/**
 * \addtogroup dsekai_engines_types_sect Engine Types
 *
 * \{
 */

#ifndef NO_ENGINE_POV

#define ENGINE_TABLE( f ) f( 0, NONE, title ) f( 1, TOPDOWN, topdown ) f( 2, POV, pov )

#else

/**
 * \brief List of available engine types.
 */
#define ENGINE_TABLE( f ) f( 0, NONE, title ) f( 1, TOPDOWN, topdown )

#endif /* ENGINE_POV */

#ifndef ENGINES_TOKENS_ONLY

/**
 * \brief Sets up the current engine (allocates specific state, etc).
 * \param state Locked ::MEMORY_PTR for current ::DSEKAI_STATE.
 * \return 1 if engine should continue running or 0 on failure.
 */
typedef int16_t (*ENGINES_SETUP)( struct DSEKAI_STATE* state );

/**
 * \brief Handles polled input according to the rules of the current engine.
 * \param in_char Last char polled from user input.
 * \param state Locked ::MEMORY_PTR for current ::DSEKAI_STATE.
 * \return 1 if engine should continue running or 0 to quit.
 */
typedef int16_t (*ENGINES_INPUT)( char in_char, struct DSEKAI_STATE* state );

/**
 * \brief Cycles animations and executes scripts relevant to the current engine.
 * \param state Locked ::MEMORY_PTR for current ::DSEKAI_STATE.
 */
typedef void (*ENGINES_ANIMATE)( struct DSEKAI_STATE* state );

/**
 * \brief Draws the engine sceen on-screen according to current state.
 * \param state Locked ::MEMORY_PTR for current ::DSEKAI_STATE.
 */
typedef void (*ENGINES_DRAW)( struct DSEKAI_STATE* state );

#define ENGINES_SETUP_PROTOTYPES( idx, eng, prefix ) int16_t prefix ## _setup( struct DSEKAI_STATE* state );

ENGINE_TABLE( ENGINES_SETUP_PROTOTYPES )

#define ENGINES_INPUT_PROTOTYPES( idx, eng, prefix ) int16_t prefix ## _input( char in_char, struct DSEKAI_STATE* state );

ENGINE_TABLE( ENGINES_INPUT_PROTOTYPES )

#define ENGINES_ANIMATE_PROTOTYPES( idx, eng, prefix ) void prefix ## _animate( struct DSEKAI_STATE* state );

ENGINE_TABLE( ENGINES_ANIMATE_PROTOTYPES )

#define ENGINES_DRAW_PROTOTYPES( idx, eng, prefix ) void prefix ## _draw( struct DSEKAI_STATE* state );

ENGINE_TABLE( ENGINES_DRAW_PROTOTYPES )

#define ENGINES_SHUTDOWN_PROTOTYPES( idx, eng, prefix ) void prefix ## _shutdown( struct DSEKAI_STATE* state );

ENGINE_TABLE( ENGINES_SHUTDOWN_PROTOTYPES )

#endif /* !ENGINES_TOKENS_ONLY */

#ifdef ENGINES_C

#define ENGINES_LIST_TOKENS( idx, eng, prefix ) #prefix,

const char* gc_engines_tokens[] = {
   ENGINE_TABLE( ENGINES_LIST_TOKENS )
   ""
};

#ifndef ENGINES_TOKENS_ONLY

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

#define ENGINES_LIST_SHUTDOWN( idx, eng, prefix ) prefix ## _shutdown,

const ENGINES_DRAW gc_engines_shutdown[] = {
   ENGINE_TABLE( ENGINES_LIST_SHUTDOWN )
};

#endif /* !ENGINES_TOKENS_ONLY */

#else

extern const char* gc_engines_tokens[];

#ifndef ENGINES_TOKENS_ONLY

extern const ENGINES_SETUP gc_engines_setup[];
extern const ENGINES_INPUT gc_engines_input[];
extern const ENGINES_ANIMATE gc_engines_animate[];
extern const ENGINES_DRAW gc_engines_draw[];

#endif /* !ENGINES_TOKENS_ONLY */

#endif /* ENGINES_C */

/*! \} */

/*! \} */

#endif /* ENGINES_H */

