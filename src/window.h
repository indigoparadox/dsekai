
#ifndef WINDOW_H
#define WINDOW_H

/*! \file window.h
 *  \brief Tools for drawing and interacting with graphical windows on-screen.
 */

#define WINDOW_CENTERED -1

#define WINDOW_COUNT_MAX 10

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
};

void window_init();
void window_shutdown();
int window_draw_all( struct DSEKAI_STATE* state );

/**
 * \brief Push a new window onto the global window stack.
 * \param id A unique identifier for the window being pushed.
 * \param status
 * \param x The left horizontal offset of the window in pixels.
 * \param y The top vertical offset of the window in pixels.
 * \param w The width of the window in pixels.
 * \param h The height of the window in pixels.
 * \param frame_idx The index of the window frame to use.
 * \param state ::MEMORY_PTR to the global engine state.
 */
int16_t window_push(
   uint32_t id, uint8_t status,
   int16_t x, int16_t y, int16_t w, int16_t h, uint8_t frame_idx,
   struct DSEKAI_STATE* state
void window_pop( uint32_t, struct DSEKAI_STATE* );
int16_t window_modal( struct DSEKAI_STATE* );

#endif /* WINDOW_H */

