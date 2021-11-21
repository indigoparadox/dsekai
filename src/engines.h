
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

/*! \brief General/shared state of the running engine in memory. */
struct DSEKAI_STATE {
   struct ITEM items[DSEKAI_ITEMS_MAX];
   uint16_t items_count;

   struct TILEMAP map;

   /*! \brief Array of currently loaded MOBILE objects on this map. */
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

};

/**
 * \relates TITLE_STATE
 * \brief Handler for ::ENGINE_TYPE_NONE.
 * \param state_handle Unlocked ::MEMORY_HANDLE for current ::DSEKAI_STATE.
 * \param args ::MEMORY_PTR to ::GRAPHICS_ARGS to be passed to graphics_flip().
 * \return 1 if engine should continue executing or 0 if it should quit.
 */
int title_loop( MEMORY_HANDLE state_handle, struct GRAPHICS_ARGS* args );

/**
 * \relates TOPDOWN_STATE
 * \brief Handler for ::ENGINE_TYPE_TOPDOWN.
 * \param state_handle Unlocked ::MEMORY_HANDLE for current ::DSEKAI_STATE.
 * \param args ::MEMORY_PTR to ::GRAPHICS_ARGS to be passed to graphics_flip().
 * \return 1 if engine should continue executing or 0 if it should quit.
 */
int topdown_loop( MEMORY_HANDLE, struct GRAPHICS_ARGS* );

/**
 * \relates POV_STATE
 * \brief Handler for ::ENGINE_TYPE_POV.
 * \param state_handle Unlocked ::MEMORY_HANDLE for current ::DSEKAI_STATE.
 * \param args ::MEMORY_PTR to ::GRAPHICS_ARGS to be passed to graphics_flip().
 * \return 1 if engine should continue executing or 0 if it should quit.
 */
int pov_loop( MEMORY_HANDLE, struct GRAPHICS_ARGS* );

/*! \} */

#endif /* ENGINES_H */

