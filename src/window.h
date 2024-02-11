
#ifndef WINDOW_H
#define WINDOW_H

/**
 * \addtogroup unilayer_gui Unilayer Graphical User Interface
 * \brief In-engine interface for windows.
 *
 * Windows are nested graphical controls that can be used to create an
 * interactive interface. There are various \ref unilayer_gui_windows_types
 * which can be used together to construct dialog boxes and menus.
 *
 * \warning This API is still heavily in development and is subject to change
 *          without notice!
 * \{
 */

/*! \file window.h
 *  \brief Tools for drawing and interacting with graphical windows on-screen.
 */

/**
 * \addtogroup unilayer_gui Unilayer Graphical User Interface
 */

#ifndef WINDOWS_MAX
/*! \brief Maximum number of windows that can be on-screen at one time. */
#define WINDOWS_MAX 100
#endif /* !WINDOWS_MAX */

#ifndef WINDOW_PADDING_OUTSIDE
/*! \brief Internally-used padding in pixels from controls to window border. */
#define WINDOW_PADDING_OUTSIDE  10
#endif /* !WINDOW_PADDING_OUTSIDE */

#ifndef WINDOW_PADDING_INSIDE
/*! \brief Internally-used padding in pixels between controls. */
#define WINDOW_PADDING_INSIDE   2
#endif /* !WINDOW_PADDING_INSIDE */

#ifndef WINDOW_SPRITE_W
#define WINDOW_SPRITE_W 16
#endif /* !WINDOW_SPRITE_W */

#ifndef WINDOW_SPRITE_H
#define WINDOW_SPRITE_H 16
#endif /* !WINDOW_SPRITE_H */

#ifndef WINDOW_PATTERN_W
#define WINDOW_PATTERN_W 16
#endif /* !WINDOW_PATTERN_W */

#ifndef WINDOW_PATTERN_H
#define WINDOW_PATTERN_H 16
#endif /* !WINDOW_PATTERN_H */

/*! \} */

/*! \brief A piece of data attached to CONTROL::data. */
union CONTROL_DATA {
   int32_t scalar;
   MAUG_MHANDLE string;
   int16_t grid[4];
};

/**
 * \brief Defines a graphical window frame/background with a set of image
 *        assets.
 */
struct WINDOW_FRAME {
   /*! \brief Top-right corner. */
   uint16_t tr;
   /*! \brief Top-left corner. */
   uint16_t tl;
   /*! \brief Bottom-right corner. */
   uint16_t br;
   /*! \brief Bottom-left corner. */
   uint16_t bl;
   /*! \brief Top edge. */
   uint16_t t;
   /*! \brief Bottom edge. */
   uint16_t b;
   /*! \brief Right edge. */
   uint16_t r;
   /*! \brief Left edge. */
   uint16_t l;
   /*! \brief Center fill. */
   uint16_t c;
};

/*! \brief Struct representing an on-screen graphical window. */
struct WINDOW {
   /*! \brief Unique identifier used to find this window in the global stack. */
   uint16_t id;
   /*! \brief Unique identifier of the parent window of this window. */
   uint16_t parent_id;
   /*! \brief Type of window defined by a constant. */
   uint8_t type;
   /*! \brief \ref unilayer_gui_windows_flags modifying window behavior. */
   uint8_t flags;
   /**
    * \brief Flags passed directly to rendering function, depending on
    *        \ref unilayer_gui_windows_types.
    */
   uint8_t render_flags;
   /**
    * \brief Foreground \ref unilayer_graphics_colors with which to draw this
    *        window's text.
    */
   RETROFLAT_COLOR fg;
   /**
    * \brief Background \ref unilayer_graphics_colors with which to draw this
    *        window.
    */
   RETROFLAT_COLOR bg;
   /**
    * \brief Pixel coordinates for this window on-screen; a tuple indexed as
    *        ::GUI_X, ::GUI_Y, ::GUI_W, ::GUI_H.
    *
    *        Please see \ref unilayer_gui_windows_placement for more
    *        information on window placement and sizing.
    */
   uint16_t coords[4];
   /*! \brief Data specific to each \ref unilayer_gui_windows_types. */
   union CONTROL_DATA data;
};

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

/**
 * \brief Size of internal text buffer used to represent
 *        ::WINDOW_FLAG_TEXT_NUM.
 */
#define WINDOW_NUM_BUFFER_SZ       10

/**
 * \addtogroup unilayer_gui_windows_flags GUI Window Flags
 * \brief Options specifying how a ::WINDOW behaves.
 *
 * The lower 4 bits are general to all \ref unilayer_gui_windows_types while
 * the upper 4 bits are specific to each ::WINDOW:type.
 *
 * \{
 */

/**
 * \brief WINDOW::flags indicating window is visible and interactive.
 */
#define WINDOW_FLAG_ACTIVE       0x01
#define WINDOW_FLAG_DIRTY  0x04
/**
 * \brief WINDOW::status indicating window is blocking all input.
 */
#define WINDOW_FLAG_MODAL  0x08
/**
 * \brief WINDOW::flags indicating window option has been selected.
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

/*! \} */ /* unilayer_gui_windows_flags */

#define WINDOW_STRING_SZ_MAX 100

/**
 * \addtogroup unilayer_gui_windows_placement GUI Window Placement
 * \brief Options for specifying ::WINDOW placement in a ::WINDOW.
 *
 * \{
 */

/*! \brief WINDOW::x or WINDOW::y value indicating the system should do its
 *         best to center the WINDOW onscreen.
 */
#define WINDOW_PLACEMENT_CENTER           0x8000

/**
 * \brief Alight the window's right side (if specified as X) or bottom side
 *        (if specified as Y) to the window's respective side. */
#define WINDOW_PLACEMENT_RIGHT_BOTTOM     0x2000

/**
 * \brief Place the window at the grid X or Y as relevant, and set the grid
 *        width or height respectively at the window's width or height.
 */
#define WINDOW_PLACEMENT_GRID_RIGHT_DOWN  0xc000

/**
 * \brief Place the window at the grid X or Y without modifying the grid.
 */
#define WINDOW_PLACEMENT_GRID             0x4000

/**
 * \brief Bitmask for WINDOW::coords fields ::GUI_X and ::GUI_Y that specifies
 *        automatic placement flags.
 */
#define WINDOW_PLACEMENT_AUTO_MASK        0xe000

/**
 * \brief Bitmask for WINDOW::coords fields ::GUI_X and ::GUI_Y that specifies
 *        physical placement coordinate number.
 *        Inverse of ::WINDOW_SIZE_AUTO_MASK.
 */
#define WINDOW_PLACEMENT_PHYS_MASK        0x1fff

#define WINDOW_SIZE_AUTO                  0x8000

/**
 * \brief Bitmask for WINDOW::coords fields ::GUI_W and ::GUI_H that specifies
 *        automatic sizing flags.
 */
#define WINDOW_SIZE_AUTO_MASK             (WINDOW_PLACEMENT_AUTO_MASK)

/**
 * \brief Bitmask for WINDOW::coords fields ::GUI_W and ::GUI_H that specifies
 *        physical sizing coordinate number.
 *        Inverse of ::WINDOW_SIZE_AUTO_MASK.
 */
#define WINDOW_SIZE_PHYS_MASK             (WINDOW_PLACEMENT_PHYS_MASK)

#define window_screen_reset_grid() memory_zero_ptr( g_window_screen_grid, 4 * sizeof( int16_t ) );

/**
 * \brief Set the physical coordinates of a window without corrupting attached
 *        flags.
 * \param window Locked ::MEMORY_PTR to the window to modify.
 */
#define window_update_coords( window, x_y_w_h, coord ) (window)->coords[x_y_w_h] = ((window)->coords[x_y_w_h] & WINDOW_PLACEMENT_AUTO_MASK) | (WINDOW_PLACEMENT_PHYS_MASK & (coord));

/**
 * \brief Get the physical coordinates of a window without flags attached.
 * \param window Locked ::MEMORY_PTR to the window to reference.
 */
#define window_get_coords( window, x_y_w_h ) (((window)->coords[x_y_w_h] & WINDOW_PLACEMENT_PHYS_MASK))

/*! \} */ /* unilayer_gui_windows_placement */

#ifdef WINDOW_TRACE
#  define window_trace_printf( lvl, ... ) debug_printf( lvl, __VA_ARGS__ )
#else
/**
 * \brief Alias for debug_printf() enabled when WINDOW_TRACE is defined.
 *
 * This was made its own alias since the GUI can get rather chatty when it's
 * enabled!
 */
#  define window_trace_printf( ... )
#endif /* WINDOW_TRACE */

#ifdef DEPTH_VGA
#  define WINDOW_PREFAB_DEFAULT_FG() RETROFLAT_COLOR_WHITE
#  define WINDOW_PREFAB_DEFAULT_BG() RETROFLAT_COLOR_MAGENTA
#  define WINDOW_PREFAB_DEFAULT_HL() RETROFLAT_COLOR_CYAN
#elif defined( DEPTH_CGA )
#  define WINDOW_PREFAB_DEFAULT_FG() RETROFLAT_COLOR_BLACK
#  define WINDOW_PREFAB_DEFAULT_BG() RETROFLAT_COLOR_MAGENTA
#  define WINDOW_PREFAB_DEFAULT_HL() RETROFLAT_COLOR_CYAN
#elif defined( DEPTH_MONO )
   /* TODO: Verify this looks OK. */
#  define WINDOW_PREFAB_DEFAULT_FG() RETROFLAT_COLOR_WHITE
#  define WINDOW_PREFAB_DEFAULT_BG() RETROFLAT_COLOR_BLACK
#  define WINDOW_PREFAB_DEFAULT_HL() RETROFLAT_COLOR_WHITE
#endif /* DEPTH */

/**
 * \brief Global initialization for the window subsystem. Runs at startup.
 * \param auto_w Limit width of screen area for auto-centering.
 * \param auto_h Limit height of screen area for auto-centering.
 * \return 1 if successful, or 0 otherwise.
 */
int16_t window_init(
   uint16_t auto_x, uint16_t auto_y, uint16_t auto_w, uint16_t auto_h
) SECTION_WINDOW;

int16_t window_reload_frames() SECTION_WINDOW;

/**
 * \brief Global shutdown for the window subsystem. Runs at shutdown.
 */
void window_shutdown() SECTION_WINDOW;

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
   uint16_t x, uint16_t y, uint16_t w, uint16_t h,
   RETROFLAT_COLOR fg, RETROFLAT_COLOR bg, uint8_t render_flags,
   int32_t data_scalar, const char* data_string ) SECTION_WINDOW;

/**
 * \brief Destroy the top-most onscreen WINDOW with the given WINDOW::id.
 * \param id WINDOW::id to search for and eliminate.
 * \param windows Locked ::MEMORY_PTR to the global window list.
 */
void window_pop( uint16_t id ) SECTION_WINDOW;

void window_refresh( uint16_t w_id ) SECTION_WINDOW;

#if 0
/**
 * \brief Determine if there is a WINDOW_FLAG_MODAL WINDOW on-screen.
 * \param windows Locked ::MEMORY_PTR to the global window list.
 * \return 0 if no modal windows showing, 1+ otherwise.
 */
int16_t window_modal();
#endif

#define window_modal() (g_window_modals)

/**
 * \addtogroup unilayer_gui_windows_types GUI Window Types
 * \brief Window interaction modes.
 * \{
 */

/**
 * \brief \b WINDOW_TYPE_WINDOW: A parent container to group subordinate
 *        windows with a graphical background.
 *
 * WINDOW::render_flags: The index of the ::WINDOW_FRAME to use.
 */
#define WINDOW_CB_TABLE_4( f ) f( 4, WINDOW )

/**
 * \brief \b WINDOW_TYPE_SPRITE: A sprite from a ::GRAPHICS_BITMAP spritesheet.
 */
#define WINDOW_CB_TABLE_3( f ) f( 3, SPRITE ) WINDOW_CB_TABLE_4( f )

/**
 * \brief \b WINDOW_TYPE_CHECK: A toggleable checkbox.
 *
 * \warning Not yet implemented!
 */
#define WINDOW_CB_TABLE_2( f ) f( 2, CHECK ) WINDOW_CB_TABLE_3( f  )

/**
 * \brief \b WINDOW_TYPE_BUTTON: A clickable button.
 *
 * \warning Not yet implemented!
 */
#define WINDOW_CB_TABLE_1( f ) f( 1, BUTTON ) WINDOW_CB_TABLE_2( f )

/**
 * \brief \b WINDOW_TYPE_LABEL: A static text label.
 *
 * WINDOW::render_flags : These are \ref unilayer_graphics_string_flags passed
 * directly to graphics_string_at().
 */
#define WINDOW_CB_TABLE( f ) f( 0, LABEL ) WINDOW_CB_TABLE_1( f )

/*! \} */ /* unilayer_gui_windows_types */

typedef int16_t (*WINDOW_CB_DRAW)( uint16_t w_id, struct WINDOW* windows );

/**
 * \return 1 if successful and 0 otherwise.
 */
typedef uint8_t (*WINDOW_CB_SZ)(
   uint16_t w_id, struct WINDOW* windows, uint8_t w_h, uint16_t* out );

#ifdef WINDOW_C

static MAUG_MHANDLE g_frames_handle = (MAUG_MHANDLE)NULL;
static MAUG_MHANDLE g_windows_handle = (MAUG_MHANDLE)NULL;

uint8_t g_window_modals = 0;

static uint16_t g_window_screen_coords[4] = {
   0, 0, SCREEN_W, SCREEN_H
};

static int16_t g_window_screen_grid[4] = {
   0, 0, 0, 0
};

#  define WINDOW_CB_DRAW_TABLE_PROTOTYPES( idx, name ) int16_t window_draw_ ## name( uint16_t w_id, struct WINDOW* windows );

WINDOW_CB_TABLE( WINDOW_CB_DRAW_TABLE_PROTOTYPES );

#  define WINDOW_CB_SZ_TABLE_PROTOTYPES( idx, name ) uint8_t window_sz_ ## name( uint16_t w_id, struct WINDOW* windows, uint8_t w_h, uint16_t* out ) SECTION_WINDOW;

WINDOW_CB_TABLE( WINDOW_CB_SZ_TABLE_PROTOTYPES );

#  define WINDOW_CB_DRAW_TABLE_LIST( idx, name ) window_draw_ ## name,

static const WINDOW_CB_DRAW gc_window_draw_callbacks[] = {
   WINDOW_CB_TABLE( WINDOW_CB_DRAW_TABLE_LIST )
};

#  define WINDOW_CB_SZ_TABLE_LIST( idx, name ) window_sz_ ## name,

static const WINDOW_CB_SZ gc_window_sz_callbacks[] = {
   WINDOW_CB_TABLE( WINDOW_CB_SZ_TABLE_LIST )
};

#  define WINDOW_CB_TABLE_CONSTS( idx, name ) MAUG_CONST uint16_t WINDOW_TYPE_ ## name = idx;

WINDOW_CB_TABLE( WINDOW_CB_TABLE_CONSTS );

#else

extern uint8_t g_window_modals;

/**
 * \brief Define extern constants that can be used e.g. in spawners.
 */
#  define WINDOW_CB_TABLE_CONSTS( idx, name ) extern MAUG_CONST uint16_t WINDOW_TYPE_ ## name;

WINDOW_CB_TABLE( WINDOW_CB_TABLE_CONSTS )

#endif /* WINDOW_C */

/*! \} */

#endif /* WINDOW_H */

