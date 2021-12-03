
#ifndef GUSTRUCT_H
#define GUSTRUCT_H

/**
 * \addtogroup dsekai_gui
 *
 * \{
 */

 /*! \file gustruct.h
  *  \brief Structures representing GUI elements.
  */

/*! \brief A piece of data attached to CONTROL::data. */
union CONTROL_DATA {
   int32_t scalar;
   /*! \brief Identifier for an asset (e.g. for CONTROL_TYPE_SPRITE). */
   RESOURCE_ID res_id;
   char* string;
};

/*! \brief A graphical UI element, typically attached to a WINDOW. */
struct CONTROL {
   /*! \brief Unique identifier used to find this control in a window stack. */
   uint32_t id;
   /*! \brief Type of control defined by a constant. */
   uint16_t type;
#if 0
   /**
    * \brief The left horizontal offset of the control in pixels.
    *
    * Relative to containing window.
    */
   int16_t x;
   /**
    * \brief The top vertical offset of the control in pixels.
    *
    * Relative to containing window.
    */
   int16_t y;
   /*! \brief The width of the control in pixels. */
   int16_t w;
   /*! \brief The height of the control in pixels. */
   int16_t h;
#endif
   /**
    * \brief Control coordinates relative to parent WINDOW and dimensions 
    *        indexed as ::GUI_X, ::GUI_Y, ::GUI_W, ::GUI_H.
    */
   int16_t coords[4];
   /*! \deprecated */
   int8_t scale;
   GRAPHICS_COLOR fg;
   GRAPHICS_COLOR bg;
   /*! \brief Data used to draw a control (e.g. text or sprite to display). */
   union CONTROL_DATA data;
   /*! \brief Control-specific flags. */
   uint8_t flags;
};

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
#if 0
   /*! \brief The left horizontal offset of the window in pixels. */
   int16_t x;
   /*! \brief The top vertical offset of the window in pixels. */
   int16_t y;
   /*! \brief The width of the window in pixels. */
   int16_t w;
   /*! \brief The height of the window in pixels. */
   int16_t h;
   int16_t grid_x;
   int16_t grid_x_prev;
   int16_t grid_y;
   int16_t grid_y_prev;
#endif
   /**
    * \brief Window coordinates and dimensions indexed as ::GUI_X, ::GUI_Y,
    *        ::GUI_W, ::GUI_H.
    */
   int16_t coords[4];
   int16_t grid[4];
};

/*! \} */

#endif /* !GUSTRUCT_H */

