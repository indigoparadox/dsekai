
#ifndef CONTROL_H
#define CONTROL_H

/**
 * \addtogroup dsekai_gui
 *
 * \{
 */

/*! \file control.h
 *  \brief Tools for drawing and interacting with graphical UI elements.
 */

/**
 * \addtogroup dsekai_gui_controls_sect GUI Control Types
 * \brief Types of controls that can be drawn on windows.
 *
 * \{
 *
 * \page dsekai_gui_controls GUI Control Types
 *
 * \section dsekai_gui_controls_button CONTROL_TYPE_BUTTON
 * Selectable control to perform an action.
 *
 * \section dsekai_gui_controls_label_t CONTROL_TYPE_LABEL
 * Static text display field.
 * \subsection dsekai_gui_controls_label_t_scalar Scalar Data
 * TODO: FIXME
 * Index of string in TILEMAP::strings.
 *
 * \section dsekai_gui_controls_check CONTROL_TYPE_CHECK
 * Selectable control to indicate an option.
 *
 * \section dsekai_gui_controls_sprite CONTROL_TYPE_SPRITE
 * Static image display field.
 * \subsection dsekai_gui_controls_sprite_res Resource ID
 * ::RESOURCE_ID of image to display.
 *
 * \}
 */

/**
 * \relates WINDOW
 * \brief Maximum number of controls permitted in WINDOW::controls_handle.
 */
#define CONTROL_COUNT_MAX           20

/**
 * \addtogroup dsekai_gui_controls_placement GUI Control Placement
 * \brief Options for specifying ::CONTROL placement in a ::WINDOW.
 *
 * \{
 */

/*! \brief Place the control in the center of the window. */
#define CONTROL_PLACEMENT_CENTER       (-1)
/*! \brief Alight the control's right side to the window's right side. */
#define CONTROL_PLACEMENT_RIGHT        (-2)
/*! \brief Alight the control's bottom side to the window's bottom side. */
#define CONTROL_PLACEMENT_BOTTOM       (-2)
/**
 * \brief Place the control at the grid X, and set the grid width at the
 *        control's width.
 */
#define CONTROL_PLACEMENT_GRID_RIGHT   (-3)
/**
 * \brief Place the control at the grid Y, and set the grid height at the
 *        control's height.
 */
#define CONTROL_PLACEMENT_GRID_DOWN    (-3)
/**
 * \brief Place the control at the grid X or Y without modifying the grid.
 */
#define CONTROL_PLACEMENT_GRID         (-4)

/*! \} */

/*! \brief Internally-used padding in pixels from controls to window border. */
#define CONTROL_PADDING_OUTSIDE  10
/*! \brief Internally-used padding in pixels between controls. */
#define CONTROL_PADDING_INSIDE   2

/**
 * \addtogroup dsekai_gui_controls_flags GUI Control Flags
 * \brief Options specifying how a ::CONTROL behaves.
 *
 * The lower 4 bits are general to all controls while the upper 4 bits are
 * specific to each ::CONTRO:type.
 *
 * \{
 */

/**
 * \relates CONTROL
 * \brief CONTROL::flags indicating control is visible and interactive.
 */
#define CONTROL_FLAG_ENABLED       0x01
/**
 * \relates CONTROL
 * \brief CONTROL::flags indicating control has cursor focus.
 */
#define CONTROL_FLAG_ACTIVE        0x02
/**
 * \relates CONTROL
 * \brief CONTROL::flags indicating control option has been selected.
 */
#define CONTROL_FLAG_CHECKED       0x10
/**
 * \relates CONTROL
 * \brief CONTROL::flags for CONTROL_TYPE_LABEL indicating text comes from
 *        ::TILEMAP:strpool.
 */
#define CONTROL_FLAG_TEXT_TILEMAP   0x10
/**
 * \relates CONTROL
 * \brief CONTROL::flags for CONTROL_TYPE_LABEL indicating text comes from
 *        ::gc_menu_tokens.
 */
#define CONTROL_FLAG_TEXT_MENU      0x20
/**
 * \relates CONTROL
 * \brief CONTROL::flags for CONTROL_TYPE_LABEL indicating text comes from
 *        the name of an item in ::DSEKAI_STATE:items.
 */
#define CONTROL_FLAG_TEXT_ITEM      0x40
/**
 * \relates CONTROL
 * \brief CONTROL::flags for CONTROL_TYPE_LABEL indicating text is a number.
 */
#define CONTROL_FLAG_TEXT_NUM       0x80

/*! \} */

/**
 * \brief Size of internal text buffer used to represent
 *        ::CONTROL_FLAG_TEXT_NUM.
 */
#define CONTROL_NUM_BUFFER_SZ       10

/*! \brief A piece of data attached to CONTROL::data. */
union CONTROL_DATA {
   int32_t scalar;
   /*! \brief Identifier for an asset (e.g. for CONTROL_TYPE_SPRITE). */
   RESOURCE_ID res_id;
};

/*! \brief A graphical UI element, typically attached to a WINDOW. */
struct CONTROL {
   /*! \brief Unique identifier used to find this control in a window stack. */
   uint32_t id;
   /*! \brief Type of control defined by a constant. */
   uint16_t type;
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
   /*! \brief Control-specific flags. */
   uint8_t flags;
};

#define CONTROL_CB_TABLE( f ) f( 0, LABEL ) f( 1, BUTTON ) f( 2, CHECK ) f( 3, SPRITE )

typedef int16_t (*CONTROL_CB_DRAW)(
   struct WINDOW* w, struct CONTROL* c, struct DSEKAI_STATE* state );

/**
 * \return 1 if successful and 0 otherwise.
 */
typedef uint8_t (*CONTROL_CB_SZ)(
   struct WINDOW* w, struct CONTROL* c, struct DSEKAI_STATE* state,
   struct GRAPHICS_RECT* r );

int16_t control_push(
   uint32_t control_id, uint16_t type, uint8_t flags,
   int16_t x, int16_t y, int16_t w, int16_t h,
   GRAPHICS_COLOR fg, GRAPHICS_COLOR bg, int8_t scale,
   int32_t data_scalar, RESOURCE_ID data_res_id,
   uint32_t window_id, struct DSEKAI_STATE* state );

/**
 * \brief Remove the control with the given ID from the control stack for the
 *        given window.
 */
void control_pop( uint32_t, uint32_t, struct DSEKAI_STATE* );
void control_draw_all( struct WINDOW* w, struct DSEKAI_STATE* state );

#ifndef PLATFORM_PALM

#ifdef CONTROL_C

/* === If we're being called inside control.c === */

#define CONTROL_CB_DRAW_TABLE_PROTOTYPES( idx, name ) static int16_t control_draw_ ## name( struct WINDOW*, struct CONTROL*, struct DSEKAI_STATE* state );

CONTROL_CB_TABLE( CONTROL_CB_DRAW_TABLE_PROTOTYPES );

#define CONTROL_CB_SZ_TABLE_PROTOTYPES( idx, name ) static uint8_t control_sz_ ## name( struct WINDOW*, struct CONTROL*, struct DSEKAI_STATE* state, struct GRAPHICS_RECT* );

CONTROL_CB_TABLE( CONTROL_CB_SZ_TABLE_PROTOTYPES );

#define CONTROL_CB_DRAW_TABLE_LIST( idx, name ) control_draw_ ## name,

static const CONTROL_CB_DRAW gc_control_draw_callbacks[] = {
   CONTROL_CB_TABLE( CONTROL_CB_DRAW_TABLE_LIST )
};

#define CONTROL_CB_SZ_TABLE_LIST( idx, name ) control_sz_ ## name,

static const CONTROL_CB_SZ gc_control_sz_callbacks[] = {
   CONTROL_CB_TABLE( CONTROL_CB_SZ_TABLE_LIST )
};

#define CONTROL_CB_TABLE_CONSTS( idx, name ) const uint16_t CONTROL_TYPE_ ## name = idx;

CONTROL_CB_TABLE( CONTROL_CB_TABLE_CONSTS );

#else

/* === If we're being called inside anything BUT control.c === */

/**
 * \brief Define extern constants that can be used e.g. in spawners.
 */
#define CONTROL_CB_TABLE_CONSTS( idx, name ) extern const uint16_t CONTROL_TYPE_ ## name;

CONTROL_CB_TABLE( CONTROL_CB_TABLE_CONSTS );

#endif /* CONTROL_C */

#endif /* !PLATFORM_PALM */

/*! \} */

#endif /* CONTROL_H */

