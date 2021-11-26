
#ifndef WINDOW_H
#define WINDOW_H

/**
 * \addtogroup dsekai_gui Graphical User Interface
 * \brief In-engine interface for windows and controls.
 *
 * \{
 */

/*! \{ */

/*! \file window.h
 *  \brief Tools for drawing and interacting with graphical windows on-screen.
 */

/*! \brief WINDOW::x or WINDOW::y value indicating the system should do its
 *         best to center the WINDOW onscreen.
 */
#define WINDOW_CENTERED -1

/*! \brief Maximum number of windows that can be onscreen at one time. */
#define WINDOW_COUNT_MAX 10

/*! \brief Recommended WINDOW::id for the welcome window. */
#define WINDOW_ID_WELCOME 0x1212
/*! \brief Recommended WINDOW::id for the status window. */
#define WINDOW_ID_STATUS 0x111
/*! \brief Recommended WINDOW::id for a script speech window. */
#define WINDOW_ID_SCRIPT_SPEAK 0x897

/*! \brief WINDOW::status indicating window is hidden/inactive. */
#define WINDOW_STATUS_EMPTY    0
/*! \brief WINDOW::status indicating window is being animated open. */
#define WINDOW_STATUS_OPENING  1
/*! \brief WINDOW::status indicating window is visible normally. */
#define WINDOW_STATUS_VISIBLE  2
/*! \brief WINDOW::status indicating window is blocking all input. */
#define WINDOW_STATUS_MODAL  3

/**
 * \brief Defines a graphical window frame/background with a set of image
 *        assets.
 */
struct WINDOW_FRAME {
   /*! \brief Top-right corner. */
   RESOURCE_ID tr;
   /*! \brief Top-left corner. */
   RESOURCE_ID tl;
   /*! \brief Bottom-right corner. */
   RESOURCE_ID br;
   /*! \brief Bottom-left corner. */
   RESOURCE_ID bl;
   /*! \brief Top edge. */
   RESOURCE_ID t;
   /*! \brief Bottom edge. */
   RESOURCE_ID b;
   /*! \brief Right edge. */
   RESOURCE_ID r;
   /*! \brief Left edge. */
   RESOURCE_ID l;
   /*! \brief Center fill. */
   RESOURCE_ID c;
};

/*! \brief Struct representing an on-screen graphical window. */
struct WINDOW {
   /*! \brief Unique identifier used to find this window in the global stack. */
   uint32_t id;
   /*! \brief Current drawing status of this window. */
   uint8_t status;
   /*! \brief 1 indicates window must be redrawn, 0 otherwise. */
   uint8_t dirty;
   uint8_t frame_idx;
   /*! \brief Number of controls in WINDOW::controls_handle. */
   uint8_t controls_count;
   /*! \brief List of CONTROL to draw inside this window. */
   MEMORY_HANDLE controls_handle;
   /*! \brief The left horizontal offset of the window in pixels. */
   int16_t x;
   /*! \brief The top vertical offset of the window in pixels. */
   int16_t y;
   /*! \brief The width of the window in pixels. */
   int16_t w;
   /*! \brief The height of the window in pixels. */
   int16_t h;
   int16_t grid_x;
   int16_t grid_y;
};

#ifdef PLATFORM_PALM

#define window_prefab_dialog( id, dialog, sprite, state, fg, bg )

#else

/**
 * \brief Convenience macro for creating a dialog ::WINDOW with a sprite and
 *        text string specified from TILEMAP::strings.
 * \image html windowsp.png
 * \param id WINDOW::id for this window.
 * \param dialog Index of the string to display from TILEMAP::strings.
 * \param sprite RESOURCE_ID of the GRAPHICS_BITMAP to display in this window.
 * \param state Current global ::DSEKAI_STATE.
 * \param state ::MEMORY_PTR to the global engine ::DSEKAI_STATE.
 */
#define window_prefab_dialog( id, dialog, sprite, state, fg, bg ) window_push( id, WINDOW_STATUS_MODAL, SCREEN_MAP_X, WINDOW_CENTERED, SCREEN_MAP_W, 64, 0, state ); control_push( 0x2323, CONTROL_TYPE_LABEL_T, CONTROL_STATE_ENABLED, CONTROL_PLACEMENT_CENTER, 30, CONTROL_PLACEMENT_CENTER, CONTROL_PLACEMENT_CENTER, fg, bg, 1, dialog, 0, id, state, state->map.strings ); control_push( 0x2324, CONTROL_TYPE_SPRITE, CONTROL_STATE_ENABLED, CONTROL_PLACEMENT_CENTER, 6, CONTROL_PLACEMENT_CENTER, CONTROL_PLACEMENT_CENTER, fg, bg, 1, 0, sprite, id, state, state->map.strings ); 

#endif /* PLATFORM_PALM */

/**
 * \brief Global initialization for the window subsystem. Runs at startup.
 */
void window_init();

/**
 * \brief Global shutdown for the window subsystem. Runs at shutdown.
 */
void window_shutdown();

int window_draw_all( struct DSEKAI_STATE* state );

/**
 * \brief Push a new window onto the global window stack.
 * \param id WINDOW::id for the window being pushed.
 * \param status Initial WINDOW::status for the window being pushed.
 * \param x The left horizontal offset of the window in pixels.
 * \param y The top vertical offset of the window in pixels.
 * \param w The width of the window in pixels.
 * \param h The height of the window in pixels.
 * \param frame_idx The index of the window frame to use.
 * \param state ::MEMORY_PTR to the global engine ::DSEKAI_STATE.
 */
int16_t window_push(
   uint32_t id, uint8_t status,
   int16_t x, int16_t y, int16_t w, int16_t h, uint8_t frame_idx,
   struct DSEKAI_STATE* state );

/**
 * \brief Destroy the top-most onscreen WINDOW with the given WINDOW::id.
 * \param id WINDOW::id to search for and eliminate.
 * \param state ::MEMORY_PTR to the global engine ::DSEKAI_STATE.
 */
void window_pop( uint32_t id, struct DSEKAI_STATE* state );

/**
 * \brief Determine if there is a WINDOW_STATUS_MODAL WINDOW on-screen.
 * \param state ::MEMORY_PTR to the global engine ::DSEKAI_STATE.
 * \return 0 if no modal windows showing, 1+ otherwise.
 */
int16_t window_modal( struct DSEKAI_STATE* );

/*! \} */

#endif /* WINDOW_H */

