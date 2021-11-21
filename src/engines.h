
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

/*! \brief Display the title screen. */
#define ENGINE_TYPE_NONE 0
/*! \brief Use the topdown 2D engine. */
#define ENGINE_TYPE_TOPDOWN 1
/*! \brief Use the POV 3D engine. */
#define ENGINE_TYPE_POV 2

/**
 * \addtogroup dsekai_engines_title DSekai Title Engine
 * \brief Simple title screen engine.
 *
 * \{
 */

/*! \brief State for ::ENGINE_TYPE_NONE */
struct TITLE_STATE {
   /*! \brief Index of the currently highlighted option. */
   uint8_t option_high;
};

/**
 * \brief Handler for ::ENGINE_TYPE_NONE.
 * \param state_handle Unlocked ::MEMORY_HANDLE for current ::DSEKAI_STATE.
 * \param args ::MEMORY_PTR to ::GRAPHICS_ARGS to be passed to graphics_flip().
 * \return 1 if engine should continue executing or 0 if it should quit.
 */
int title_loop( MEMORY_HANDLE state_handle, struct GRAPHICS_ARGS* args );

/*! \} */

/**
 * \addtogroup dsekai_engines_topdown DSekai Top-Down Engine
 * \brief Displays a top-down 2-dimensional view of the world.
 *
 * \{
 */

/*! \brief State for ::ENGINE_TYPE_TOPDOWN */
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
 * \brief Handler for ::ENGINE_TYPE_TOPDOWN.
 * \param state_handle Unlocked ::MEMORY_HANDLE for current ::DSEKAI_STATE.
 * \param args ::MEMORY_PTR to ::GRAPHICS_ARGS to be passed to graphics_flip().
 * \return 1 if engine should continue executing or 0 if it should quit.
 */
int topdown_loop( MEMORY_HANDLE, struct GRAPHICS_ARGS* );

/*! \} */

/**
 * \addtogroup dsekai_engines_pov DSekai POV Engine
 * \brief (Indev) Displays a first-person 3D view of the world using raycasting.
 *
 * \{
 */

/*! \brief State for ::ENGINE_TYPE_POV */
struct POV_STATE {
   double plane_y;
};

/**
 * \brief Handler for ::ENGINE_TYPE_POV.
 * \param state_handle Unlocked ::MEMORY_HANDLE for current ::DSEKAI_STATE.
 * \param args ::MEMORY_PTR to ::GRAPHICS_ARGS to be passed to graphics_flip().
 * \return 1 if engine should continue executing or 0 if it should quit.
 */
int pov_loop( MEMORY_HANDLE, struct GRAPHICS_ARGS* );

/*! \} */

/*! \brief General/shared state of the running engine in memory. */
struct PACKED DSEKAI_STATE {
   struct ITEM items[DSEKAI_ITEMS_MAX];
   uint16_t items_count;

   struct TILEMAP map;

   /*! \brief Array of currently loaded MOBILE objects on this map. */
   struct MOBILE mobiles[DSEKAI_MOBILES_MAX];
   struct MOBILE player;

   MEMORY_HANDLE engine_state_handle;

   uint8_t input_blocked_countdown;
   uint8_t ani_sprite_countdown;
   uint16_t ani_sprite_x;

   MEMORY_HANDLE windows_handle;
   uint16_t windows_count;

   char warp_to[TILEMAP_NAME_MAX];
   uint8_t warp_to_x;
   uint8_t warp_to_y;

   uint16_t engine_state;

};

/*! \} */

#endif /* ENGINES_H */

