
#ifndef MENU_H
#define MENU_H

/**
 * \addtogroup dsekai_menus Menus
 * \brief On-screen menu handling world-specific functions.
 *
 * \{
 */

#define MENU_TABLE( f ) f( main ) f( items ) f( quit )

#define MENU_WINDOW_ID 786
#define MENU_TEXT_SZ 20

void menu_open( struct DSEKAI_STATE* state );

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

#define MENU_FLAG_DIRTY 0x01

struct MENU_STATE {
   int8_t menu_id;
   int8_t highlight_id;
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

extern const MENU_RENDERER gc_menu_renderers[];
extern const MENU_HANDLER gc_menu_handlers[];
extern const char gc_menu_tokens[][MENU_TEXT_SZ];

#endif /* MENU_C */

/*! \} */

#endif /* !MENU_H */

