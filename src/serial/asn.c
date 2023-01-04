
#define SERIAL_C
#include "../dsekai.h"

/* Private Prototypes w/ Sections */

int32_t serial_write_mobile(
   MEMORY_HANDLE* p_save_buffer_h, int32_t idx, struct MOBILE* m ) SECTION_SETUP;

int32_t serial_read_mobile(
   MEMORY_HANDLE* p_save_buffer_h, int32_t idx, struct MOBILE* m ) SECTION_SETUP;

/* Function Definitions */

int32_t serial_write_mobile(
   MEMORY_HANDLE* p_save_buffer_h, int32_t idx, struct MOBILE* m
) {
   int32_t mark_seq_mob = 0,
      i = 0;

   serial_asn_write_seq_start(
      p_save_buffer_h, &mark_seq_mob, "mobile", idx, cleanup );

   serial_asn_write_int(
      p_save_buffer_h, m->flags, "mobile flags", idx, cleanup );
   serial_asn_write_int(
      p_save_buffer_h, m->spawner_gid, "mobile spawner GID", idx, cleanup );
   serial_asn_write_int(
      p_save_buffer_h, m->map_gid, "mobile tilemap GID", idx, cleanup );
   serial_asn_write_int(
      p_save_buffer_h, m->mp_hp, "mobile MP/HP", idx, cleanup );
   serial_asn_write_int(
      p_save_buffer_h, m->screen_px, "mobile screen pixel X", idx, cleanup );
   serial_asn_write_int(
      p_save_buffer_h, m->screen_py, "mobile screen pixel Y", idx, cleanup );
   serial_asn_write_int(
      p_save_buffer_h, m->ascii, "mobile ASCII", idx, cleanup );
   serial_asn_write_int(
      p_save_buffer_h, m->coords.x, "mobile tile X", idx, cleanup );
   serial_asn_write_int(
      p_save_buffer_h, m->coords.y, "mobile tile Y", idx, cleanup );
   serial_asn_write_int(
      p_save_buffer_h, m->coords_prev.x, "mobile previous tile X", idx, cleanup );
   serial_asn_write_int(
      p_save_buffer_h, m->coords_prev.y, "mobile previous tile Y", idx, cleanup );
   serial_asn_write_int(
      p_save_buffer_h, m->steps_remaining,
      "mobile steps remaining", idx, cleanup );
   serial_asn_write_int(
      p_save_buffer_h, m->script_id, "mobile script ID", idx, cleanup );
   serial_asn_write_int(
      p_save_buffer_h, m->script_pc, "mobile script PC", idx, cleanup );
   for( i = 0 ; SCRIPT_STACK_DEPTH > i ; i++ ) {
      serial_asn_write_int(
         p_save_buffer_h, m->script_stack[i],
         "mobile script stack", idx, cleanup );
   }
   serial_asn_write_int(
      p_save_buffer_h, m->script_wait_frames,
      "mobile script wait frames", idx, cleanup );

   serial_asn_write_seq_end(
      p_save_buffer_h, &mark_seq_mob, "mobile", idx, cleanup );

cleanup:

   return idx;
}

int32_t serial_save( const char* save_name, struct DSEKAI_STATE* state ) {
   MEMORY_HANDLE save_buffer_h = (MEMORY_HANDLE)NULL;
   int32_t idx = 0;
   struct TILEMAP* t = NULL;
   uint8_t i = 0;
   int32_t mark_seq_main = 0,
      mark_seq_mobs = 0;

   /* Allocate save buffer. */
   save_buffer_h = memory_alloc( TILEMAP_ASN_SAVE_BUFFER_INITIAL_SZ, 1 );
   if( (MEMORY_HANDLE)NULL == save_buffer_h ) {
      idx = DSEKAI_ERROR_ALLOCATE;
      error_printf( "could not allocate save_buffer_h!" );
      goto cleanup;
   }

   serial_asn_write_seq_start(
      &save_buffer_h, &mark_seq_main, "main", idx, cleanup );

   serial_asn_write_int(
      &save_buffer_h, state->version, "engine version", idx, cleanup );
   serial_asn_write_int(
      &save_buffer_h, state->engine_type, "engine type", idx, cleanup );

   serial_asn_write_seq_start(
      &save_buffer_h, &mark_seq_mobs, "mobiles", idx, cleanup );

   for( i = 0 ; DSEKAI_MOBILES_MAX > i ; i++ ) {
      idx = serial_write_mobile( &save_buffer_h, idx, &(state->mobiles[i]) );
      if( 0 > idx ) {
         goto cleanup;
      }
   }

   serial_asn_write_seq_end(
      &save_buffer_h, &mark_seq_mobs, "mobiles", idx, cleanup );

   /* TODO: Serialize graphics cache index > resource_id mappings. */
   
   /* map */
   t = (struct TILEMAP*)memory_lock( state->map_handle );
   assert( NULL != t );
   idx = tilemap_asn_save( save_buffer_h, idx, t );

   serial_asn_write_seq_end(
      &save_buffer_h, &mark_seq_main, "main", idx, cleanup );

   /* Write the buffer to disk. */
   save_write( save_name, save_buffer_h, idx );

cleanup:

   if( NULL != t ) {
      t = (struct TILEMAP*)memory_unlock( state->map_handle );
   }

   if( (MEMORY_HANDLE)NULL != save_buffer_h ) {
      memory_free( save_buffer_h );
   }

   return idx;
}

int32_t serial_read_mobile(
   MEMORY_HANDLE* p_save_buffer_h, int32_t idx, struct MOBILE* m
) {
   int32_t i = 0;
   uint8_t* save_buffer = NULL;
   int32_t read_sz = 0,
      mob_seq_sz = 0;
   uint8_t type_buf = 0;

   save_buffer = memory_lock( *p_save_buffer_h );
   if( NULL == save_buffer ) {
      error_printf( "could not lock save_buffer!" );
      idx = SERIAL_ERROR;
      goto cleanup;
   }

   read_sz = asn_read_meta_ptr( save_buffer, idx, &type_buf, &mob_seq_sz );
   if( ASN_SEQUENCE != type_buf ) {
      error_printf( "invalid mobile sequence type: 0x%02x", type_buf );
      idx = SERIAL_ERROR;
      goto cleanup;
   }
   idx += read_sz;
   assert( 0 < mob_seq_sz );

   serial_asn_read_int(
      save_buffer, &(m->flags), 2, 0, "mobile flags", idx, read_sz, cleanup );
   serial_asn_read_int(
      save_buffer, &(m->spawner_gid), 2, 0,
      "mobile spawner GID", idx, read_sz, cleanup );
   serial_asn_read_int(
      save_buffer, &(m->map_gid), 2, 0,
      "mobile tilemap GID", idx, read_sz, cleanup );
   serial_asn_read_int(
      save_buffer, &(m->mp_hp), 2, ASN_FLAG_SIGNED,
      "mobile MP/HP", idx, read_sz, cleanup );
   serial_asn_read_int(
      save_buffer, &(m->screen_px), 2, ASN_FLAG_SIGNED,
      "mobile screen pixel X", idx, read_sz, cleanup );
   serial_asn_read_int(
      save_buffer, &(m->screen_py), 2, ASN_FLAG_SIGNED,
      "mobile screen pixel Y", idx, read_sz, cleanup );
   serial_asn_read_int(
      save_buffer, &(m->ascii), 1, 0, "mobile ASCII", idx, read_sz, cleanup );
   serial_asn_read_int(
      save_buffer, &(m->coords.x), 1, 0,
      "mobile tile X", idx, read_sz, cleanup );
   serial_asn_read_int(
      save_buffer, &(m->coords.y), 1, 0,
      "mobile tile Y", idx, read_sz, cleanup );
   serial_asn_read_int(
      save_buffer, &(m->coords_prev.x), 1, 0,
      "mobile previous tile X", idx, read_sz, cleanup );
   serial_asn_read_int(
      save_buffer, &(m->coords_prev.y), 1, 0,
      "mobile previous tile X", idx, read_sz, cleanup );
   serial_asn_read_int(
      save_buffer, &(m->steps_remaining), 1, 0,
      "mobile steps remaining", idx, read_sz, cleanup );
   serial_asn_read_int(
      save_buffer, &(m->script_id), 2, ASN_FLAG_SIGNED,
      "mobile script ID", idx, read_sz, cleanup );
   serial_asn_read_int(
      save_buffer, &(m->script_pc), 2, ASN_FLAG_SIGNED,
      "mobile script PC", idx, read_sz, cleanup );
   for( i = 0 ; SCRIPT_STACK_DEPTH > i ; i++ ) {
      serial_asn_read_int(
         save_buffer, &(m->script_stack[i]), 1, ASN_FLAG_SIGNED,
         "mobile script stack", idx, read_sz, cleanup );
   }
   serial_asn_read_int(
      save_buffer, &(m->script_wait_frames), 2, 0,
      "mobile script wait frames", idx, read_sz, cleanup );

cleanup:

   if( NULL != save_buffer ) {
      save_buffer = memory_unlock( *p_save_buffer_h );
   }

   return idx;
}

int32_t serial_load( const char* save_name, struct DSEKAI_STATE* state ) {
   int32_t idx = 0;
   MEMORY_HANDLE save_buffer_h = (MEMORY_HANDLE)NULL;
   uint8_t* save_buffer = NULL;
   int32_t save_buffer_sz = 0;
   uint8_t type_buf = 0;
   int32_t read_sz = 0,
      mobs_seq_sz = 0,
      save_seq_sz = 0,
      i = 0;
   uint8_t version = 0;
   struct TILEMAP* t = NULL;

   engines_draw_loading_screen();

   /* Read the buffer from disk. */
   save_buffer_h = save_read( save_name );
   if( (MEMORY_HANDLE)NULL == save_buffer_h ) {
      error_printf( "unable to load save from save subsystem!" );
      idx = SERIAL_ERROR;
      goto cleanup;
   }
   
   save_buffer_sz = memory_sz( save_buffer_h );
   assert( 0 < save_buffer_sz );
   save_buffer = memory_lock( save_buffer_h );

   serial_asn_read_seq( save_buffer, &type_buf, &save_seq_sz,
      "main", idx, read_sz, cleanup )

   if( save_buffer_sz - idx < save_seq_sz ) {
      error_printf( "invalid save sequence size: %d (max %d)",
         save_seq_sz, save_buffer_sz - idx );
   }

   serial_asn_read_int(
      save_buffer, &version, 1, 0,
      "engine version", idx, read_sz, cleanup );

   /* We can't load newer saves! */
   assert( version <= state->version );

   serial_asn_read_int(
      save_buffer, &(state->engine_type), 1, 0,
      "engine type", idx, read_sz, cleanup );

   serial_asn_read_seq( save_buffer, &type_buf, &mobs_seq_sz,
      "mobiles", idx, read_sz, cleanup )

   /* Lock save buffer so serial_read_mobile() can read it. */
   save_buffer = memory_unlock( save_buffer_h );

   /* TODO: Base this on the save mobile count, not the engine mobile count! */
   for( i = 0 ; DSEKAI_MOBILES_MAX > i ; i++ ) {
      idx = serial_read_mobile( &save_buffer_h, idx, &(state->mobiles[i]) );
      if( 0 > idx ) {
         goto cleanup;
      }
   }
   debug_printf( 2, "serialized mobiles loaded!" );

   t = (struct TILEMAP*)memory_lock( state->map_handle );
   assert( NULL != t );

   read_sz = tilemap_asn_load( &(save_buffer[idx]), save_buffer_sz - idx, t );

   t = (struct TILEMAP*)memory_unlock( state->map_handle );

   /* TODO
   memory_strncpy_ptr( state->warp_to, stringize( ENTRY_MAP ), \
      memory_strnlen_ptr( stringize( ENTRY_MAP ), TILEMAP_NAME_MAX ) ); \
      state->engine_type_change = idx;
   */

cleanup:

   if( NULL != save_buffer ) {
      save_buffer = memory_unlock( save_buffer_h );
   }

   if( (MEMORY_HANDLE)NULL != save_buffer_h ) {
      memory_free( save_buffer_h );
   }

   return idx;
}

