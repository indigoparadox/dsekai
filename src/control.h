
#ifndef CONTROL_H
#define CONTROL_H

/*! \file control.h
 *  \brief Tools for drawing and interacting with graphical UI elements.
 *
 *  | Type                | Description                                 | data_scalar                          | data_res_id                       |
 *  |---------------------|---------------------------------------------|--------------------------------------|-----------------------------------|
 *  | CONTROL_TYPE_BUTTON | Selectable control to perform an action.    |                                      |                                   |
 *  | CONTROL_TYPE_LABEL_T| Static text display field.                  | Index of string in TILEMAP::strings. |                                   |
 *  | CONTROL_TYPE_LABEL_G| Static text display field.                  | Index of string in ::gc_static_strings. |                                   |
 *  | CONTROL_TYPE_CHECK  | Selectable control to indicate an option.   |                                      |                                   |
 *  | CONTROL_TYPE_SPRITE | Static image display field.                 |                                      | ::RESOURCE_ID of image to display.|
 *
 */

#define CONTROL_COUNT_MAX           20

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
};

#define CONTROL_CB_TABLE( f ) f( 0, LABEL_T ) f( 1, BUTTON ) f( 2, CHECK ) f( 3, SPRITE ) f( 4, LABEL_G )

typedef int16_t (*CONTROL_CB_DRAW)(
   struct WINDOW* w, struct CONTROL* c,
   const char strings[][TILEMAP_STRINGS_SZ] );

/**
 * \return 1 if successful and 0 otherwise.
 */
typedef uint8_t (*CONTROL_CB_SZ)(
   struct WINDOW* w, struct CONTROL* c, struct GRAPHICS_RECT* r,
   const char strings[][TILEMAP_STRINGS_SZ] );

int16_t control_push(
   uint32_t control_id, uint16_t type, uint16_t status,
   int16_t x, int16_t y, int16_t w, int16_t h,
   GRAPHICS_COLOR fg, GRAPHICS_COLOR bg, int8_t scale,
   int32_t data_scalar, RESOURCE_ID data_res_id,
   uint32_t window_id, struct DSEKAI_STATE* state,
   const char strings[][TILEMAP_STRINGS_SZ] );
void control_pop( uint32_t, uint32_t, struct DSEKAI_STATE* );
void control_draw_all(
   struct WINDOW* w,
   const char strings[][TILEMAP_STRINGS_SZ] );

#ifndef PLATFORM_PALM

#ifdef CONTROL_C

/* === If we're being called inside control.c === */

#define CONTROL_CB_DRAW_TABLE_PROTOTYPES( idx, name ) static int16_t control_draw_ ## name( struct WINDOW*, struct CONTROL*, const char[][TILEMAP_STRINGS_SZ] );

CONTROL_CB_TABLE( CONTROL_CB_DRAW_TABLE_PROTOTYPES );

#define CONTROL_CB_SZ_TABLE_PROTOTYPES( idx, name ) static uint8_t control_sz_ ## name( struct WINDOW*, struct CONTROL*, struct GRAPHICS_RECT*, const char[][TILEMAP_STRINGS_SZ] );

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

#endif /* CONTROL_H */

