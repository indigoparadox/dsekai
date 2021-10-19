
#ifndef CONTROL_H
#define CONTROL_H

/*! \file control.h
 *  \brief Tools for drawing and interacting with graphical UI elements.
 */

#define CONTROL_COUNT_MAX           20

/**
 * \brief CONTROL::type used to display static text.
 *
 * Requires a ::MEMORY_PTR containing the string to display to be attached to
 * the accompanying CONTROL::data member.
 */
#define CONTROL_TYPE_LABEL          0
/*! \brief CONTROL::type used to indicate a potential action. */
#define CONTROL_TYPE_BUTTON         1
/*! \brief CONTROL::type used to indicate a selectable option. */
#define CONTROL_TYPE_CHECK          2
/**
 * \brief CONTROL::type used to display a sprite asset.
 *
 * Requires a ::RESOURCE_ID identifying the sprite to display to be attached to
 * the accompanying CONTROL::data member.
 */
#define CONTROL_TYPE_SPRITE         3

/*! \brief CONTROL::status indicating control is hidden/inactive. */
#define CONTROL_STATE_NONE          0
/*! \brief CONTROL::status indicating control is visible and interactive. */
#define CONTROL_STATE_ENABLED       1
/*! \brief CONTROL::status indicating control has cursor focus. */
#define CONTROL_STATE_ACTIVE        2
/*! \brief CONTROL::status indicating control option has been selected. */
#define CONTROL_STATE_CHECKED       3

/*! \brief A piece of data attached to CONTROL::data. */
union CONTROL_DATA {
   uint32_t scalar;
   MEMORY_HANDLE handle;
   MEMORY_PTR ptr;
   /*! \brief Identifier for an asset (e.g. for CONTROL_TYPE_SPRITE). */
   RESOURCE_ID res_id;
};

/*! \brief A graphical UI element, typically attached to a WINDOW. */
struct CONTROL {
   /*! \brief Unique identifier used to find this control in a window stack. */
   uint32_t id;
   /*! \brief Type of control defined by a constant. */
   uint16_t type;
   /*! \brief Current drawing status of this control. */
   uint16_t status;
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
   int8_t scale;
   GRAPHICS_COLOR fg;
   GRAPHICS_COLOR bg;
   /*! \brief Data used to draw a control (e.g. text or sprite to display). */
   union CONTROL_DATA data;
   uint16_t data_sz;
};

typedef int16_t (*CONTROL_CB)( struct WINDOW*, struct CONTROL* );

int16_t control_push(
   uint32_t control_id, uint16_t type, uint16_t status,
   int16_t x, int16_t y, int16_t w, int16_t h,
   GRAPHICS_COLOR fg, GRAPHICS_COLOR bg, int8_t scale,
   MEMORY_PTR data_ptr, uint16_t data_ptr_sz,
   uint32_t data_scalar,
   RESOURCE_ID data_res_id,
   uint32_t window_id, struct DSEKAI_STATE* state );
void control_pop( uint32_t, uint32_t, struct DSEKAI_STATE* );
void control_draw_all( struct WINDOW* );

#endif /* CONTROL_H */

