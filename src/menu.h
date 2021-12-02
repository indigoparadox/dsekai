
#ifndef MENU_H
#define MENU_H

/**
 * \addtogroup dsekai_menus Menus
 * \brief On-screen menu handling world-specific functions.
 *
 * \{
 */

/**
 * \brief Defines the top-level menu items and callbacks used to handle each
 *        submenu.
 */
#define MENU_TABLE( f ) f( main ) f( items ) f( quit )

/*! \brief WINDOW::id that can safely be used by on-screen menus. */
#define MENU_WINDOW_ID 786
#define MENU_WINDOW_INFO_ID 787
#define MENU_WINDOW_STATUS_ID 788

/**
 * \brief Maximum length of a menu item's display text.
 */
#define MENU_TEXT_SZ 20

/**
 * \brief Open the on-screen menu, pausing the current engine.
 */
void menu_open( struct DSEKAI_STATE* state );

/**
 * \brief Close the on-screen menu, resuming the current engine.
 */
void menu_close( struct DSEKAI_STATE* state );

/**
 * \brief Render a currently active menu on-screen.
 */
typedef void (*MENU_RENDERER)( struct DSEKAI_STATE* state );

/**
 * \brief Handle input for a currently active menu.
 */
typedef int16_t (*MENU_HANDLER)( char in_char, struct DSEKAI_STATE* state );

#define MENU_RENDERER_PROTOTYPES( name ) void menu_renderer_ ## name( struct DSEKAI_STATE* state );

MENU_TABLE( MENU_RENDERER_PROTOTYPES )

#define MENU_HANDLER_PROTOTYPES( name ) int16_t menu_handler_ ## name( char in_char, struct DSEKAI_STATE* state );

MENU_TABLE( MENU_HANDLER_PROTOTYPES )

/**
 * \relates MENU_STATE
 * \brief MENU_STATE::flags indicating menu draw callback should be called
 *        again.
 */
#define MENU_FLAG_DIRTY 0x01

struct MENU_STATE {
   /*! \brief Currently open menu as an index in the MENU_TABLE. */
   int8_t menu_id;
   /*! \brief Currently selected item. Managed by the callback of the menu
    *         indicated by MENU_STATE::menu_id.
    */
   int8_t highlight_id;
   /*! \brief Bitfield used to manage menu behavior. */
   uint8_t flags;
};

#ifdef MENU_C

#define MENU_TABLE_RENDERERS( name ) menu_renderer_ ## name,

const MENU_RENDERER gc_menu_renderers[] = {
   MENU_TABLE( MENU_TABLE_RENDERERS )
};

#define MENU_TABLE_HANDLERS( name ) menu_handler_ ## name,

const MENU_HANDLER gc_menu_handlers[] = {
   MENU_TABLE( MENU_TABLE_HANDLERS )
};

#define MENU_TABLE_TOKENS( name ) #name,

const char gc_menu_tokens[][MENU_TEXT_SZ] = {
   MENU_TABLE( MENU_TABLE_TOKENS )
   ""
};

#else

/**
 * \brief Table of callbacks used to draw menus.
 *
 * These are called once when the menu is opened, and should create a ::WINDOW.
 */
extern const MENU_RENDERER gc_menu_renderers[];

/**
 * \brief Table of callacks used to handle menu input.
 *
 * These should close the menu's ::WINDOW and set MENU_FLAG_DIRTY on global
 * MENU_STATE.
 */
extern const MENU_HANDLER gc_menu_handlers[];

/**
 * \brief Table of menu item names as strings.
 */
extern const char gc_menu_tokens[][MENU_TEXT_SZ];

#endif /* MENU_C */

/*! \} */

#endif /* !MENU_H */

