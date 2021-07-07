
#ifndef CONTROL_H
#define CONTROL_H

#define CONTROL_COUNT_MAX           20

#define CONTROL_TYPE_LABEL          0
#define CONTROL_TYPE_BUTTON         1
#define CONTROL_TYPE_CHECK          2

#define CONTROL_STATE_NONE          0
#define CONTROL_STATE_ENABLED       1
#define CONTROL_STATE_ACTIVE        2
#define CONTROL_STATE_CHECKED       3

struct CONTROL {
   uint32_t id;
   uint16_t type;
   uint16_t status;
   uint16_t x; /* Relative to containing window. */
   uint16_t y; /* Relative to containing window. */
   uint16_t w;
   uint16_t h;
   MEMORY_HANDLE data;
};

int16_t control_push(
   uint32_t, uint16_t, uint16_t, uint16_t, uint16_t, uint16_t, uint16_t,
   MEMORY_HANDLE, uint32_t, struct DSEKAI_STATE* );
void control_pop( uint32_t, uint32_t, struct DSEKAI_STATE* );

#endif /* CONTROL_H */

