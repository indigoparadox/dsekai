
#ifndef CONTROL_H
#define CONTROL_H

#define CONTROL_COUNT_MAX           20

#define CONTROL_TYPE_LABEL          0
#define CONTROL_TYPE_BUTTON         1
#define CONTROL_TYPE_CHECK          2
#define CONTROL_TYPE_SPRITE         3

#define CONTROL_STATE_NONE          0
#define CONTROL_STATE_ENABLED       1
#define CONTROL_STATE_ACTIVE        2
#define CONTROL_STATE_CHECKED       3

union CONTROL_DATA {
   uint32_t scalar;
   MEMORY_HANDLE handle;
};

struct CONTROL {
   uint32_t id;
   uint16_t type;
   uint16_t status;
   int16_t x; /* Relative to containing window. */
   int16_t y; /* Relative to containing window. */
   int16_t w;
   int16_t h;
   int8_t scale;
   GRAPHICS_COLOR fg;
   GRAPHICS_COLOR bg;
   union CONTROL_DATA data;
};

int16_t control_push(
   uint32_t, uint16_t, uint16_t, int16_t, int16_t, int16_t, int16_t,
   GRAPHICS_COLOR, GRAPHICS_COLOR, int8_t,
   MEMORY_HANDLE, uint32_t, uint32_t, struct DSEKAI_STATE* );
void control_pop( uint32_t, uint32_t, struct DSEKAI_STATE* );
void control_draw_all( struct WINDOW* );

#endif /* CONTROL_H */

