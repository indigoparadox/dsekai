
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
#define MENU_TABLE( f ) f( 0, main ) f( 1, items ) f( 2, craft ) f( 3, quit )

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

#define MENU_RENDERER_PROTOTYPES( idx, name ) void menu_renderer_ ## name( struct DSEKAI_STATE* state );

MENU_TABLE( MENU_RENDERER_PROTOTYPES )

#define MENU_HANDLER_PROTOTYPES( idx, name ) int16_t menu_handler_ ## name( char in_char, struct DSEKAI_STATE* state );

MENU_TABLE( MENU_HANDLER_PROTOTYPES )

/**
 * \relates MENU_STATE
 * \brief MENU_STATE::flags indicating menu draw callback should be called
 *        again.
 */
#define MENU_FLAG_DIRTY 0x01

/**
 * \relates MENU_STATE
 * \brief MENU_STATE::flags indicating item menu should only show craftable
 *        items and return to craft menu when item is selected.
 */
#define MENU_FLAG_ITEM_CRAFTABLE    0x02

struct MENU_STATE {
   /*! \brief Currently open menu as an index in the MENU_TABLE. */
   int8_t menu_id;
   /*! \brief Currently selected item. Managed by the callback of the menu
    *         indicated by MENU_STATE::menu_id.
    */
   int8_t highlight_id;
   /*! \brief Bitfield used to manage menu behavior. */
   uint8_t flags;
   /*! \brief graphics_get_ms() at which menu was last opened. */
   uint32_t open_ms;
   int8_t craft_item_ids[3];
   int8_t craft_mod_idx;
};

#ifdef MENU_C

#define MENU_TABLE_RENDERERS( idx, name ) menu_renderer_ ## name,

RES_CONST MENU_RENDERER gc_menu_renderers[] = {
   MENU_TABLE( MENU_TABLE_RENDERERS )
};

#define MENU_TABLE_HANDLERS( idx, name ) menu_handler_ ## name,

RES_CONST MENU_HANDLER gc_menu_handlers[] = {
   MENU_TABLE( MENU_TABLE_HANDLERS )
};

#define MENU_TABLE_TOKENS( idx, name ) #name,

RES_CONST char gc_menu_tokens[][MENU_TEXT_SZ] = {
   MENU_TABLE( MENU_TABLE_TOKENS )
   ""
};

#define MENU_TABLE_CONSTS( idx, name ) RES_CONST uint8_t gc_menu_idx_ ## name = idx;

MENU_TABLE( MENU_TABLE_CONSTS )

#else

/**
 * \brief Table of callbacks used to draw menus.
 *
 * These are called once when the menu is opened, and should create a ::WINDOW.
 */
extern RES_CONST MENU_RENDERER gc_menu_renderers[];

/**
 * \brief Table of callacks used to handle menu input.
 *
 * These should close the menu's ::WINDOW and set MENU_FLAG_DIRTY on global
 * MENU_STATE.
 */
extern RES_CONST MENU_HANDLER gc_menu_handlers[];

/**
 * \brief Table of menu item names as strings.
 */
extern RES_CONST char gc_menu_tokens[][MENU_TEXT_SZ];

#define MENU_TABLE_CONSTS( idx, name ) extern RES_CONST uint8_t gc_menu_idx_ ## name;

MENU_TABLE( MENU_TABLE_CONSTS )

#endif /* MENU_C */

/*! \} */

#endif /* !MENU_H */

