
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

/*! \} */ /* dsekai_engines_transitions */

#if defined( DEPTH_VGA )
#  define DSEKAI_TITLE_TEXT_COLOR GRAPHICS_COLOR_DARKRED
#elif defined( DEPTH_CGA )
#  define DSEKAI_TITLE_TEXT_COLOR GRAPHICS_COLOR_MAGENTA
#else
#  define DSEKAI_TITLE_TEXT_COLOR GRAPHICS_COLOR_WHITE
#endif /* DEPTH_VGA || DEPTH_CGA */

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

typedef void (*title_option_cb)( struct DSEKAI_STATE* state );

/**
 * \brief State for ::ENGINE_TYPE_NONE. A simple title screen engine.
 */
struct TITLE_STATE {
   RES_CONST char** option_tokens;
   title_option_cb* option_callbacks;
   /*! \brief Index of the currently highlighted option. */
   uint8_t option_idx;
   uint8_t option_min;
   uint8_t option_max;
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

/*! \} */ /* dsekai_engines_specific_struct */

#endif /* !NO_ENGINE_EDITOR */

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

/**
 * \brief General/shared state of the running engine in memory.
 *
 * \attention engines_asn_save() and engines_asn_load() MUST be updated
 *            manually when changes are made to this struct in order to
 *            serialize properly.
 */
struct DSEKAI_STATE {
   uint8_t version;

   /**
    * \brief Array consisting of all items in the current game world.
    *
    * Items in this array are considered to "exist" when the ITEM::flags
    * field has the ::ITEM_FLAG_ACTIVE bit set. Inactive items may be replaced
    * by new items.
    */
   MEMORY_HANDLE items_handle;

   /**
    * \brief Currently loaded ::TILEMAP.
    */
   MEMORY_HANDLE map_handle;

   /**
    * \brief Engine type tilemap is supposed to be used with.
    */
   uint8_t engine_type;

   /**
    * \brief Engine type set to be changed to with engines_warp_loop().
    *
    * \attention This field should not be serialized!
    */
   uint8_t engine_type_change;

   /**
    * \brief Array of currently loaded MOBILE objects on this map.
    *
    * Mobiles in this array are considered to "exist" when
    * ::MOBILE_FLAG_ACTIVE is enabled in their MOBILE::flags.
    *
    * \warning Index in this array is persistant and unique in the world!
    *          Be sure to preserve it (including empty spots) during serialize!
    */
   struct MOBILE mobiles[DSEKAI_MOBILES_MAX];

   /**
    * \brief Currently active player MOBILE. Stays between maps.
    *
    * This is kept separately from DSEKAI_STATE::mobiles because... TODO
    */
   struct MOBILE player;

   /**
    * \brief Contains the currently loaded \ref dsekai_engines_specific_struct.
    */
   MEMORY_HANDLE engine_state_handle;

   /** \brief The number of loops until DSEKAI_STATE::ani_sprite_countdown
    *         changes.
    *
    * \attention This field should not be serialized!
    */
   uint8_t ani_sprite_countdown;

   /**
    * \brief The horizontal offset of all on-screen ::MOBILE sprites on their
    *        spritesheets in pixels.
    *
    * \attention This field should not be serialized!
    */
   uint16_t ani_sprite_x;

   /**
    * \brief When holding a valid string, triggers the map change process.
    *
    * \attention This field should not be serialized!
    */
   char warp_to[TILEMAP_NAME_MAX];
   /**
    * \brief Sets the player MOBILE::coords horizontal on map change.
    *
    * \attention This field should not be serialized!
    */
   uint8_t warp_to_x;
   /**
    * \brief Sets the player MOBILE::coords vertical on map change.
    *
    * \attention This field should not be serialized!
    */
   uint8_t warp_to_y;

   /*! \brief Current engine state (see below in struct reference). */
   uint16_t engine_state;

   /*! \brief Global boolean values dictating engine state and behavior. */
   uint8_t flags;

   /**
    * \brief The current frame of the playing \ref dsekai_engines_transitions.
    *
    * If set to 0, then no transition is currently playing.
    *
    * If a transition is currently playing, then input and scripts are blocked.
    *
    * \attention This field should not be serialized!
    */
   uint8_t transition;

   /**
    * \brief Resource ID for the player sprite.
    *
    * This is necessary since the player is spawned in the first map, but
    * travels to other maps (unlike other mobiles).
    */
   RESOURCE_NAME player_sprite_name;

   /**
    * \brief Array of all crops growing on all maps.
    * 
    * This is kept as part of the state so that crops can continue growing in
    * the background.
    */
   struct CROP_PLOT crops[DSEKAI_CROPS_MAX];

#ifndef NO_ENGINE_EDITOR

   struct EDITOR_STATE editor;

#endif /* !NO_ENGINE_EDITOR */

   struct MENU_STATE menu;
};

/**
 * \brief Draw a simple loading screen.
 */
void engines_draw_loading_screen();

/**
 * \brief Handle transition to a new ::TILEMAP.
 * \param state_handle Unlocked ::MEMORY_HANDLE for current ::DSEKAI_STATE.
 */
int16_t engines_warp_loop( MEMORY_HANDLE state_handle );

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
#ifdef PLATFORM_WASM
void engines_loop_iter( void* state_handle );
#else
int16_t engines_loop_iter( MEMORY_HANDLE state_handle );
#endif /* PLATFORM_WASM */

/**
 * \brief Handle generic player movement commmand.
 * \param dir_move
 * \param state Locked ::MEMORY_PTR for current ::DSEKAI_STATE.
 * \param map Locked ::MEMORY_PTR for DSEKAI_STATE::map_handle.
 */
int8_t engines_input_movement(
   struct MOBILE* mover, int8_t dir_move,
   struct DSEKAI_STATE* state, struct TILEMAP* t );

#ifndef NO_TITLE

void engines_exit_to_title( struct DSEKAI_STATE* state );

#endif /* !NO_TITLE */

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

/* \brief Pause scripts if modal window is pending, screen is scrolling, menu
 *        is open, etc.
 */
#define engines_active( state ) (0 == window_modal() && DSEKAI_FLAG_INPUT_BLOCKED != (DSEKAI_FLAG_INPUT_BLOCKED & (state)->flags) && 0 > (state)->menu.menu_id)

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
typedef int16_t (*ENGINES_INPUT)(
   INPUT_VAL in_char, int16_t click_x, int16_t click_y,
   struct DSEKAI_STATE* state );

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

#define ENGINES_SETUP_PROTOTYPES( idx, eng, prefix ) int16_t prefix ## _setup( struct DSEKAI_STATE* state ) SECTION_SETUP;

ENGINE_TABLE( ENGINES_SETUP_PROTOTYPES )

#define ENGINES_INPUT_PROTOTYPES( idx, eng, prefix ) int16_t prefix ## _input( INPUT_VAL in_char, int16_t click_x, int16_t click_y, struct DSEKAI_STATE* state );

ENGINE_TABLE( ENGINES_INPUT_PROTOTYPES )

#define ENGINES_ANIMATE_PROTOTYPES( idx, eng, prefix ) void prefix ## _animate( struct DSEKAI_STATE* state );

ENGINE_TABLE( ENGINES_ANIMATE_PROTOTYPES )

#define ENGINES_DRAW_PROTOTYPES( idx, eng, prefix ) void prefix ## _draw( struct DSEKAI_STATE* state );

ENGINE_TABLE( ENGINES_DRAW_PROTOTYPES )

#define ENGINES_SHUTDOWN_PROTOTYPES( idx, eng, prefix ) void prefix ## _shutdown( struct DSEKAI_STATE* state ) SECTION_SETUP;

ENGINE_TABLE( ENGINES_SHUTDOWN_PROTOTYPES )

#endif /* !ENGINES_TOKENS_ONLY */

#ifdef ENGINES_C

#define ENGINES_LIST_TOKENS( idx, eng, prefix ) #prefix,

RES_CONST char* gc_engines_tokens[] = {
   ENGINE_TABLE( ENGINES_LIST_TOKENS )
   NULL
};

#ifndef ENGINES_TOKENS_ONLY

#define ENGINES_LIST_SETUP( idx, eng, prefix ) prefix ## _setup,

RES_CONST ENGINES_SETUP gc_engines_setup[] = {
   ENGINE_TABLE( ENGINES_LIST_SETUP )
};

#define ENGINES_LIST_INPUT( idx, eng, prefix ) prefix ## _input,

RES_CONST ENGINES_INPUT gc_engines_input[] = {
   ENGINE_TABLE( ENGINES_LIST_INPUT )
};

#define ENGINES_LIST_ANIMATE( idx, eng, prefix ) prefix ## _animate,

RES_CONST ENGINES_ANIMATE gc_engines_animate[] = {
   ENGINE_TABLE( ENGINES_LIST_ANIMATE )
};

#define ENGINES_LIST_DRAW( idx, eng, prefix ) prefix ## _draw,

RES_CONST ENGINES_DRAW gc_engines_draw[] = {
   ENGINE_TABLE( ENGINES_LIST_DRAW )
};

#define ENGINES_LIST_SHUTDOWN( idx, eng, prefix ) prefix ## _shutdown,

RES_CONST ENGINES_DRAW gc_engines_shutdown[] = {
   ENGINE_TABLE( ENGINES_LIST_SHUTDOWN )
};

#endif /* !ENGINES_TOKENS_ONLY */

#else

extern RES_CONST char* gc_engines_tokens[];

#ifndef ENGINES_TOKENS_ONLY

extern RES_CONST ENGINES_SETUP gc_engines_setup[];
extern RES_CONST ENGINES_INPUT gc_engines_input[];
extern RES_CONST ENGINES_ANIMATE gc_engines_animate[];
extern RES_CONST ENGINES_DRAW gc_engines_draw[];

#endif /* !ENGINES_TOKENS_ONLY */

#endif /* ENGINES_C */

/*! \} */ /* dsekai_engines_types_sect */

/*! \} */

#endif /* ENGINES_H */

