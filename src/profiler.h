
#ifndef PROFILER_H
#define PROFILER_H

/*! \file profiler.h
 *  \brief Header containing lightweight in-module profiling tools.
 */

#ifdef DSEKAI_PROFILER

#define PROFILE_FIELDS_STRUCT( field ) int32_t field;

struct DSEKAI_PROFILE {
PROFILE_FIELDS( PROFILE_FIELDS_STRUCT );
};

static struct DSEKAI_PROFILE profile;
static int32_t profile_diff = 0;

#  define profiler_init() memory_zero_ptr( &profile, sizeof( struct DSEKAI_PROFILE ) )
#  define profiler_set() profile_diff = graphics_get_ms(); if( 0 > profile_diff ) { error_printf( "error setting profiler!" ); }
#  define profiler_incr( field ) if( 0 <= profile_diff ) { profile.field += (graphics_get_ms() - profile_diff); }

#define PROFILE_FIELDS_PRINTF( field ) debug_printf( 3, #field ": %d", profile.field );

#  define profiler_print( profiler_module ) debug_printf( 3, "=== " profiler_module " PROFILER RESULTS ===" ); PROFILE_FIELDS( PROFILE_FIELDS_PRINTF ); debug_printf( 3, "=== END " profiler_module " PROFILER RESULTS ===" );

#else

#  define profiler_print( profiler_module )
#  define profiler_init()
#  define profiler_set()
#  define profiler_incr( field )

#endif /* DSEKAI_PROFILER */

#endif /* !PROFILER_H */

