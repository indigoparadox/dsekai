
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

/**
 * \brief Specifies X coordinate in WINDOW::coords or CONTROL::coords.
 */
#define GUI_X 0
/**
 * \brief Specifies X coordinate in WINDOW::coords or CONTROL::coords.
 */
#define GUI_Y 1
/**
 * \brief Specifies width coordinate in WINDOW::coords or CONTROL::coords.
 */
#define GUI_W 2
/**
 * \brief Specifies height in WINDOW::coords or CONTROL::coords.
 */
#define GUI_H 3

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

/**
 * \relates WINDOW
 * \brief WINDOW::status indicating window is hidden/inactive.
 */
#define WINDOW_STATUS_EMPTY    0
/**
 * \relates WINDOW
 * \brief WINDOW::status indicating window is being animated open.
 */
#define WINDOW_STATUS_OPENING  1
/**
 * \relates WINDOW
 * \brief WINDOW::status indicating window is visible normally.
 */
#define WINDOW_STATUS_VISIBLE  2
/**
 * \relates WINDOW
 * \brief WINDOW::status indicating window is blocking all input.
 */
#define WINDOW_STATUS_MODAL  3

#ifdef PLATFORM_PALM

#define window_prefab_dialog( id, dialog, sprite, state, fg, bg )

#else

/**
 * \brief Convenience macro for creating a dialog ::WINDOW with a sprite and
 *        text string specified from TILEMAP::strpool.
 * \image html windowsp.png
 * \param id WINDOW::id for this window.
 * \param dialog Index of the string to display from TILEMAP::strpool.
 * \param sprite RESOURCE_ID of the GRAPHICS_BITMAP to display in this window.
 * \param state Current global ::DSEKAI_STATE.
 * \param state ::MEMORY_PTR to the global engine ::DSEKAI_STATE.
 */
#define window_prefab_dialog( id, dialog, sprite, state, fg, bg ) window_push( id, WINDOW_STATUS_MODAL, SCREEN_MAP_X, WINDOW_CENTERED, SCREEN_MAP_W, 64, 0, state ); control_push( 0x2323, CONTROL_TYPE_LABEL, CONTROL_FLAG_ENABLED | CONTROL_FLAG_TEXT_TILEMAP, CONTROL_PLACEMENT_CENTER, 30, CONTROL_PLACEMENT_CENTER, CONTROL_PLACEMENT_CENTER, fg, bg, 1, dialog, 0, id, state ); control_push( 0x2324, CONTROL_TYPE_SPRITE, CONTROL_FLAG_ENABLED, CONTROL_PLACEMENT_CENTER, 6, CONTROL_PLACEMENT_CENTER, CONTROL_PLACEMENT_CENTER, fg, bg, 1, 0, sprite, id, state ); 

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

