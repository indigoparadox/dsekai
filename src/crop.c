
#define CROPS_C
#include "dsekai.h"

void crop_grow_all( struct DSEKAI_STATE* state ) {

}

int8_t crop_get_def_idx( struct DSEKAI_STATE* state, uint8_t gid ) {
   int8_t i = 0,
      retval = -1;

   /* Check TILEMAP::crop_defs for ::CROP_DEF with same GID. */
   for( i = 0 ; TILEMAP_CROP_DEFS_MAX > i ; i++ ) {
      if( state->map.crop_defs[i].gid == gid ) {
         retval = i;
         break;
      }
   }

   if( 0 > retval ) { 
      error_printf( "unable to find this crop def on the current tilemap" );
   }

   return retval;

}

