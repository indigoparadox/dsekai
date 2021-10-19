
#ifndef SCRIPT_H
#define SCRIPT_H

/*! \file script.h
 *  \brief Structs, functions, and macros for executing in-world behavior.
 */

struct MOBILE;
struct SCRIPT;
struct TILEMAP;

/*! \brief Maximum number of steps in an individual script. */
#define SCRIPT_STEPS_MAX 16

/**
 * \brief Callback to execute a behavior action. Step in a script.
 * \param pc Current program counter for this mobile.
 * \param script Currently executing script.
 * \param t ::MEMORY_PTR to currently loaded TILEMAP.
 * \param actor ::MEMORY_PTR to MOBILE executing the action.
 * \param actee ::MEMORY_PTR to MOBILE being acted upon.
 * \param tile ::MEMORY_PTR to tilemap tile being acted upon.
 * \param arg Argument passed from script.
 * \return New value for this script's program counter (e.g. MOBILE::script_pc).
 */
typedef uint16_t (*SCRIPT_CB)(
   uint16_t pc, struct SCRIPT* script, struct TILEMAP* t,
   struct MOBILE* actor, struct MOBILE* actee, struct TILEMAP_COORDS* tile,
   struct DSEKAI_STATE* state, int16_t arg );

/**
 * \brief Contains a single instruction in a script, in SCRIPT::steps.
 *
 *  Modifications to this struct should also be reflected in tools/map2h.c.
 */
struct PACKED SCRIPT_STEP {
   uint16_t action;
   int16_t arg;
};

/**
 * \brief Contains immutable state for scripts attached to a TILEMAP.
 *
 *  Modifications to this struct should also be reflected in tools/map2h.c.
 */
struct PACKED SCRIPT {
   /*! \brief Indexed list of script steps in order. */
   struct SCRIPT_STEP steps[SCRIPT_STEPS_MAX];
   /*! \brief Number of SCRIPT::steps attached to this script. */
   uint16_t steps_count;
};

/**
 * \brief Define the script action callback table.
 * \param f Macro to execute on the function callback definition.
 */
#define SCRIPT_CB_TABLE( f ) f( 0, INTERACT, 'i' ) f( 1, WALK_NORTH, 'u' ) f( 2, WALK_SOUTH, 'd' ) f( 3, WALK_EAST, 'r' ) f( 4, WALK_WEST, 'l' ) f( 5, SLEEP, 's' ) f( 6, START, 't' ) f( 7, GOTO, 'g' ) f( 8, SPEAK, 'p' ) f( 9, RETURN, 'x' )

/*! \brief Define prototypes for the script action callbacks. */
#define SCRIPT_CB_TABLE_PROTOTYPES( idx, name, c ) uint16_t script_handle_ ## name( uint16_t, struct SCRIPT*, struct TILEMAP*, struct MOBILE*, struct MOBILE*, struct TILEMAP_COORDS*, struct DSEKAI_STATE*, int16_t );

SCRIPT_CB_TABLE( SCRIPT_CB_TABLE_PROTOTYPES )

/**
 * \brief Parse the given script string into an in-memory script.
 * \param script_txt The script string.
 * \param script_txt_sz The length of the script string.
 * \param script ::MEMORY_PTR to the SCRIPT struct to populate.
 */
uint16_t script_parse_str(
   char* script_txt, int16_t script_txt_sz, struct SCRIPT* script );

uint16_t script_goto_label(
   uint16_t pc, struct SCRIPT* script, uint16_t label_type, uint16_t label_id );

#ifdef SCRIPT_C

/* === If we're being called inside script.c === */

#define SCRIPT_CB_TABLE_LIST( idx, name, c ) script_handle_ ## name,

const SCRIPT_CB gc_script_handlers[] = {
   SCRIPT_CB_TABLE( SCRIPT_CB_TABLE_LIST )
};

#define SCRIPT_CB_TABLE_CONSTS( idx, name, c ) const uint16_t SCRIPT_ACTION_ ## name = idx;

SCRIPT_CB_TABLE( SCRIPT_CB_TABLE_CONSTS );

#else

/* === If we're being called inside anything BUT script.c === */

/**
 * \brief Define extern constants that can be used e.g. in spawners.
 */
#define SCRIPT_CB_TABLE_CONSTS( idx, name, c ) extern const uint16_t SCRIPT_ACTION_ ## name;

SCRIPT_CB_TABLE( SCRIPT_CB_TABLE_CONSTS );

extern const SCRIPT_CB gc_script_handlers[];
#endif /* !SCRIPT_C */

#endif /* SCRIPT_H */

