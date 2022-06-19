
#ifndef WINDOW_H
#define WINDOW_H

/**
 * \addtogroup dsekai_gui Graphical User Interface
 * \brief In-engine interface for windows and controls.
 *
 * \{
 */

/*! \brief A piece of data attached to CONTROL::data. */
union CONTROL_DATA {
   int32_t scalar;
   /*! \brief Identifier for an asset (e.g. for CONTROL_TYPE_SPRITE). */
   RESOURCE_ID res_id;
   MEMORY_HANDLE string;
   int16_t grid[4];
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
   uint16_t id;
   uint16_t parent_id;
   /*! \brief Type of control defined by a constant. */
   uint8_t type;
   /*! \brief Current drawing status of this window. */
   uint8_t flags;
   /* TODO: Work into flags. */
   uint8_t render_flags;
   /**
    * \brief Window coordinates and dimensions indexed as ::GUI_X, ::GUI_Y,
    *        ::GUI_W, ::GUI_H.
    */
   GRAPHICS_COLOR fg;
   GRAPHICS_COLOR bg;
   int16_t coords[4];
   union CONTROL_DATA data;
};

/*! \} */

/*! \file window.h
 *  \brief Tools for drawing and interacting with graphical windows on-screen.
 */

/**
 * \brief Specifies X coordinate in WINDOW::coords.
 */
#define GUI_X 0
/**
 * \brief Specifies X coordinate in WINDOW::coords.
 */
#define GUI_Y 1
/**
 * \brief Specifies width coordinate in WINDOW::coords.
 */
#define GUI_W 2
/**
 * \brief Specifies height in WINDOW::coords.
 */
#define GUI_H 3

/*! \brief WINDOW::x or WINDOW::y value indicating the system should do its
 *         best to center the WINDOW onscreen.
 */
#define WINDOW_CENTERED -1

/*! \brief Maximum number of windows that can be onscreen at one time. */
#define WINDOW_COUNT_MAX 10

/**
 * \brief Size of internal text buffer used to represent
 *        ::WINDOW_FLAG_TEXT_NUM.
 */
#define WINDOW_NUM_BUFFER_SZ       10

/**
 * \addtogroup dsekai_gui_controls_flags GUI Control Flags
 * \brief Options specifying how a ::WINDOW behaves.
 *
 * The lower 4 bits are general to all controls while the upper 4 bits are
 * specific to each ::WINDOW:type.
 *
 * \{
 */

/**
 * \brief WINDOW::flags indicating control is visible and interactive.
 */
#define WINDOW_FLAG_ACTIVE       0x01
#define WINDOW_FLAG_DIRTY  0x04
/**
 * \brief WINDOW::status indicating window is blocking all input.
 */
#define WINDOW_FLAG_MODAL  0x08
/**
 * \brief WINDOW::flags indicating control option has been selected.
 */
#define WINDOW_FLAG_CHECKED       0x10

/**
 * \brief WINDOW::flags for WINDOW_TYPE_LABEL indicating text is a number.
 */
#define WINDOW_FLAG_TEXT_NUM        0x10

#define WINDOW_FLAG_TEXT_PTR        0x20

#define WINDOW_FLAG_TEXT_MASK       0x30

#define WINDOW_FLAG_SPRITE_DIR_MASK 0x30

#define WINDOW_FLAG_SPRITE_SOUTH    0x00
#define WINDOW_FLAG_SPRITE_NORTH    0x10
#define WINDOW_FLAG_SPRITE_EAST     0x20
#define WINDOW_FLAG_SPRITE_WEST     0x30

#define WINDOW_FLAG_SPRITE_BORDER_SINGLE  0x40

/*! \} */

#define WINDOW_STRING_SZ_MAX 100

/**
 * \addtogroup dsekai_gui_controls_placement GUI Control Placement
 * \brief Options for specifying ::WINDOW placement in a ::WINDOW.
 *
 * \{
 */

/*! \brief Place the control in the center of the window. */
#define WINDOW_PLACEMENT_CENTER       (-1)
/**
 * \brief Alight the control's right side (if specified as X) or bottom side
 *        (if specified as Y) to the window's respective side. */
#define WINDOW_PLACEMENT_RIGHT_BOTTOM (-2)
/**
 * \brief Place the control at the grid X or Y as relevant, and set the grid
 *        width or height respectively at the control's width or height.
 */
#define WINDOW_PLACEMENT_GRID_RIGHT_DOWN   (-3)
/**
 * \brief Place the control at the grid X or Y without modifying the grid.
 */
#define WINDOW_PLACEMENT_GRID         (-4)

#define WINDOW_SIZE_AUTO   (-5)

/*! \} */

/*! \brief Internally-used padding in pixels from controls to window border. */
#define WINDOW_PADDING_OUTSIDE  10
/*! \brief Internally-used padding in pixels between controls. */
#define WINDOW_PADDING_INSIDE   2

#define window_screen_reset_grid() memory_zero_ptr( g_window_screen_grid, 4 * sizeof( int16_t ) );

#ifdef DEPTH_VGA
#  define WINDOW_PREFAB_DEFAULT_FG() GRAPHICS_COLOR_WHITE
#  define WINDOW_PREFAB_DEFAULT_BG() GRAPHICS_COLOR_MAGENTA
#  define WINDOW_PREFAB_DEFAULT_HL() GRAPHICS_COLOR_CYAN
#elif defined( DEPTH_CGA )
#  define WINDOW_PREFAB_DEFAULT_FG() GRAPHICS_COLOR_BLACK
#  define WINDOW_PREFAB_DEFAULT_BG() GRAPHICS_COLOR_MAGENTA
#  define WINDOW_PREFAB_DEFAULT_HL() GRAPHICS_COLOR_CYAN
#elif defined( DEPTH_MONO )
   /* TODO: Verify this looks OK. */
#  define WINDOW_PREFAB_DEFAULT_FG() GRAPHICS_COLOR_WHITE
#  define WINDOW_PREFAB_DEFAULT_BG() GRAPHICS_COLOR_BLACK
#  define WINDOW_PREFAB_DEFAULT_HL() GRAPHICS_COLOR_WHITE
#endif /* DEPTH */

/**
 * \brief Convenience macro for creating a dialog ::WINDOW with a sprite and
 *        text string specified from TILEMAP::strpool.
 * \image html windowsp.png
 * \param dialog Index of the string to display from TILEMAP::strpool.
 * \param sprite RESOURCE_ID of the GRAPHICS_BITMAP to display in this window.
 */
#define window_prefab_dialog( dialog, sprite, dir_flag, fg, bg ) window_push( WINDOW_ID_SCRIPT_SPEAK, 0, WINDOW_TYPE_WINDOW, WINDOW_FLAG_MODAL, SCREEN_MAP_X, WINDOW_PLACEMENT_CENTER, SCREEN_MAP_W, 64, fg, bg, 0, 0, 0, NULL ); window_push( WINDOW_ID_SCRIPT_SPEAK + 1, WINDOW_ID_SCRIPT_SPEAK, WINDOW_TYPE_LABEL, 0, WINDOW_PLACEMENT_CENTER, 30, WINDOW_PLACEMENT_CENTER, WINDOW_PLACEMENT_CENTER, fg, bg, GRAPHICS_STRING_FLAG_FONT_SCRIPT, 0, 0, dialog ); window_push( WINDOW_ID_SCRIPT_SPEAK + 2, WINDOW_ID_SCRIPT_SPEAK, WINDOW_TYPE_SPRITE, dir_flag | WINDOW_FLAG_SPRITE_BORDER_SINGLE, WINDOW_PLACEMENT_CENTER, 6, WINDOW_SIZE_AUTO, WINDOW_SIZE_AUTO, fg, bg, 0, 0, sprite, NULL ); 

#define window_prefab_system_dialog( dialog, fg, bg ) window_push( WINDOW_ID_SCRIPT_SPEAK, 0, WINDOW_TYPE_WINDOW, WINDOW_FLAG_MODAL, SCREEN_MAP_X, WINDOW_PLACEMENT_CENTER, SCREEN_MAP_W, 64, fg, bg, 0, 0, 0, NULL ); window_push( WINDOW_ID_SCRIPT_SPEAK + 1, WINDOW_ID_SCRIPT_SPEAK, WINDOW_TYPE_LABEL, 0, WINDOW_PLACEMENT_CENTER, 30, WINDOW_PLACEMENT_CENTER, WINDOW_PLACEMENT_CENTER, fg, bg, 0, 0, 0, dialog ); window_push( WINDOW_ID_SCRIPT_SPEAK + 2, WINDOW_ID_SCRIPT_SPEAK, WINDOW_TYPE_SPRITE, MOBILE_DIR_SOUTH | WINDOW_FLAG_SPRITE_BORDER_SINGLE, WINDOW_PLACEMENT_CENTER, 6, WINDOW_SIZE_AUTO, WINDOW_SIZE_AUTO, fg, bg, 0, 0, state->player.sprite, NULL ); 

/**
 * \brief Global initialization for the window subsystem. Runs at startup.
 */
void window_init();

/**
 * \brief Global shutdown for the window subsystem. Runs at shutdown.
 */
void window_shutdown();

int16_t window_draw_all();

/**
 * \brief Push a new window onto the global window stack.
 * \param id WINDOW::id for the window being pushed.
 * \param status Initial WINDOW::status for the window being pushed.
 * \param x The left horizontal offset of the window in pixels.
 * \param y The top vertical offset of the window in pixels.
 * \param w The width of the window in pixels.
 * \param h The height of the window in pixels.
 * \param frame_idx The index of the window frame to use.
 * \param windows Locked ::MEMORY_PTR to the global window list.
 */
int16_t window_push(
   uint16_t id, uint16_t parent_id, uint8_t type, uint8_t flags,
   int16_t x, int16_t y, int16_t w, int16_t h,
   GRAPHICS_COLOR fg, GRAPHICS_COLOR bg, uint8_t render_flags,
   int32_t data_scalar, RESOURCE_ID data_res_id, const char* data_string );

/**
 * \brief Destroy the top-most onscreen WINDOW with the given WINDOW::id.
 * \param id WINDOW::id to search for and eliminate.
 * \param windows Locked ::MEMORY_PTR to the global window list.
 */
void window_pop( uint16_t id );

void window_refresh( uint16_t w_id );

/**
 * \brief Determine if there is a WINDOW_FLAG_MODAL WINDOW on-screen.
 * \param windows Locked ::MEMORY_PTR to the global window list.
 * \return 0 if no modal windows showing, 1+ otherwise.
 */
int16_t window_modal();

#define WINDOW_CB_TABLE( f ) f( 0, LABEL ) f( 1, BUTTON ) f( 2, CHECK ) f( 3, SPRITE ) f( 4, WINDOW )

typedef int16_t (*WINDOW_CB_DRAW)( uint16_t w_id, struct WINDOW* windows );

/**
 * \return 1 if successful and 0 otherwise.
 */
typedef uint8_t (*WINDOW_CB_SZ)(
   uint16_t w_id, struct WINDOW* windows, int16_t r[2] );



#ifdef WINDOW_C

static MEMORY_HANDLE g_frames_handle = (MEMORY_HANDLE)NULL;
static MEMORY_HANDLE g_windows_handle = (MEMORY_HANDLE)NULL;

static const int16_t gc_window_screen_coords[4] = {
   0, 0, SCREEN_MAP_W, SCREEN_MAP_H
};

static int16_t g_window_screen_grid[4] = {
   0, 0, 0, 0
};

#define WINDOW_CB_DRAW_TABLE_PROTOTYPES( idx, name ) static int16_t window_draw_ ## name( uint16_t w_id, struct WINDOW* windows );

WINDOW_CB_TABLE( WINDOW_CB_DRAW_TABLE_PROTOTYPES );

#define WINDOW_CB_SZ_TABLE_PROTOTYPES( idx, name ) static uint8_t window_sz_ ## name( uint16_t w_id, struct WINDOW* windows, int16_t r[2] );

WINDOW_CB_TABLE( WINDOW_CB_SZ_TABLE_PROTOTYPES );

#define WINDOW_CB_DRAW_TABLE_LIST( idx, name ) window_draw_ ## name,

static const WINDOW_CB_DRAW gc_window_draw_callbacks[] = {
   WINDOW_CB_TABLE( WINDOW_CB_DRAW_TABLE_LIST )
};

#define WINDOW_CB_SZ_TABLE_LIST( idx, name ) window_sz_ ## name,

static const WINDOW_CB_SZ gc_window_sz_callbacks[] = {
   WINDOW_CB_TABLE( WINDOW_CB_SZ_TABLE_LIST )
};

#define WINDOW_CB_TABLE_CONSTS( idx, name ) RES_CONST uint16_t WINDOW_TYPE_ ## name = idx;

WINDOW_CB_TABLE( WINDOW_CB_TABLE_CONSTS );

#else

/**
 * \brief Define extern constants that can be used e.g. in spawners.
 */
#define WINDOW_CB_TABLE_CONSTS( idx, name ) extern RES_CONST uint16_t WINDOW_TYPE_ ## name;

WINDOW_CB_TABLE( WINDOW_CB_TABLE_CONSTS )

#endif /* WINDOW_C */

/*! \} */

#endif /* WINDOW_H */

