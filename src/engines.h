
#ifndef ENGINES_H
#define ENGINES_H

/*! \file engines.h
 *  \brief Macros, structs, and prototypes for the various engine types.
 */

/*! \brief (Invalid) Do not use an engine. */
#define ENGINE_TYPE_NONE 0
/*! \brief Use the topdown 2D engine. */
#define ENGINE_TYPE_TOPDOWN 1
/*! \brief Use the POV 3D engine. */
#define ENGINE_TYPE_POV 2

/*! \brief State for ::ENGINE_TYPE_NONE */
struct TITLE_STATE {
   /*! \brief Index of the currently highlighted option. */
   uint8_t option_high;
};

/*! \brief State for ::ENGINE_TYPE_TOPDOWN */
struct TOPDOWN_STATE {
   /*! \brief Left horizontal offset of the current viewport in pixels. */
   int16_t screen_scroll_x;
   /*! \brief Top vertical offset of the current viewport in pixels. */
   int16_t screen_scroll_y;
   /*! \brief Left horizontal offset of the current viewport in tiles. */
   int16_t screen_scroll_tx;
   /*! \brief Top vertical offset of the current viewport in tiles. */
   int16_t screen_scroll_ty;
   /*! \brief Horizontal offset the viewport is scrolling to in pixels. */
   int16_t screen_scroll_x_tgt;
   /*! \brief Vertical offset the viewport is scrolling to in pixels. */
   int16_t screen_scroll_y_tgt;
};

/*! \brief State for ::ENGINE_TYPE_POV */
struct POV_STATE {
   double plane_y;
};

/* \brief Handler for ::ENGINE_TYPE_NONE */
int title_loop( MEMORY_HANDLE state_handle, struct GRAPHICS_ARGS* args );
/* \brief Handler for ::ENGINE_TYPE_TOPDOWN */
int topdown_loop( MEMORY_HANDLE, struct GRAPHICS_ARGS* );
/* \brief Handler for ::ENGINE_TYPE_POV */
int pov_loop( MEMORY_HANDLE, struct GRAPHICS_ARGS* );

#endif /* ENGINES_H */

