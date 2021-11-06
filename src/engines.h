
#ifndef ENGINES_H
#define ENGINES_H

struct TITLE_STATE {
   uint8_t option_high;
};

struct POV_STATE {
   double plane_y;
};

struct TOPDOWN_STATE {
   int16_t screen_scroll_x;
   int16_t screen_scroll_y;
   int16_t screen_scroll_tx;
   int16_t screen_scroll_ty;
   int16_t screen_scroll_x_tgt;
   int16_t screen_scroll_y_tgt;
};

int topdown_loop( MEMORY_HANDLE, struct GRAPHICS_ARGS* );
int pov_loop( MEMORY_HANDLE, struct GRAPHICS_ARGS* );
int title_loop( MEMORY_HANDLE state_handle, struct GRAPHICS_ARGS* args );

#endif /* ENGINES_H */

