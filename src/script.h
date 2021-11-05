
#ifndef SCRIPT_H
#define SCRIPT_H

/*! \file script.h
 *  \brief Functions and macros for executing in-world behavior.
 */

struct MOBILE;
struct SCRIPT;
struct TILEMAP;

/*! \brief Maximum number of global script flags available. */
#define SCRIPT_GLOBALS_MAX 20
/*! \brief Maximum depth of available local stack for each ::SCRIPT. */
#define SCRIPT_STACK_DEPTH 5
/*! \brief Value of script arg to indicate real arg should be popped from stack.
 */
#define SCRIPT_ARG_STACK 32767
/*! \brief Maximum length of a script in text form. */
#define SCRIPT_STR_MAX 1024

/* TODO: This is a valid stack value. */
#define SCRIPT_ERROR_OVERFLOW -1

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

#include "scstruct.h"

/**
 * \brief Define the script action callback table.
 * \param f Macro to execute on the function callback definition.
 */
#define SCRIPT_CB_TABLE( f ) f( 0, NOOP, '\0' ) f( 1, INTERACT, 'i' ) f( 2, WALK_NORTH, 'u' ) f( 3, WALK_SOUTH, 'd' ) f( 4, WALK_EAST, 'r' ) f( 5, WALK_WEST, 'l' ) f( 6, SLEEP, 's' ) f( 7, START, 't' ) f( 8, GOTO, 'g' ) f( 9, SPEAK, 'p' ) f( 10, RETURN, 'x' ) f( 11, FACE, 'f' ) f( 12, GLOBAL_SET, 'b' ) f( 13, GLOBAL_GET, 'a' ) f( 14, WARP, 'w' )

/*! \brief Define prototypes for the script action callbacks. */
#define SCRIPT_CB_TABLE_PROTOTYPES( idx, name, c ) uint16_t script_handle_ ## name( uint16_t, struct SCRIPT*, struct TILEMAP*, struct MOBILE*, struct MOBILE*, struct TILEMAP_COORDS*, struct DSEKAI_STATE*, int16_t );

SCRIPT_CB_TABLE( SCRIPT_CB_TABLE_PROTOTYPES )

/**
 * \brief Parse the given script string into an in-memory script.
 * \param script_idx Index of the script, used for debug messages.
 * \param script_txt The script string.
 * \param script_txt_sz The length of the script string.
 * \param script ::MEMORY_PTR to the SCRIPT struct to populate.
 */
uint16_t script_parse_str(
   int script_idx,
   char* script_txt, int16_t script_txt_sz, struct SCRIPT* script );

/**
 * \brief Script subsystem init function that should be called in main().
 * \return 1 if successful or 0 otherwise.
 */
uint8_t script_init();

/**
 * \brief Script subsystem cleanup function that should be called in main().
 */
void script_shutdown();

/**
 * \brief Find a label of the given type and return the program counter
 *        (e.g. MOBILE::script_pc) at which it exists.
 * \param pc Current program counter value.
 * \param script ::SCRIPT to search for label.
 * \param label_type SCRIPT_STEP::action to consider as the label type.
 * \param label_id SCRIPT_STEP::arg to consider as the label ID.
 * \return Index of the found label, or pc if it was not found.
 */
uint16_t script_goto_label(
   uint16_t pc, struct SCRIPT* script, uint16_t label_type, uint16_t label_id );

#ifdef SCRIPT_C

/* === If we're being called inside script.c === */

int8_t g_script_globals[SCRIPT_GLOBALS_MAX];

#define SCRIPT_CB_TABLE_LIST( idx, name, c ) script_handle_ ## name,

const SCRIPT_CB gc_script_handlers[] = {
   SCRIPT_CB_TABLE( SCRIPT_CB_TABLE_LIST )
};

#define SCRIPT_CB_TABLE_CONSTS( idx, name, c ) const uint16_t SCRIPT_ACTION_ ## name = idx;

SCRIPT_CB_TABLE( SCRIPT_CB_TABLE_CONSTS );

#else

/* === If we're being called inside anything BUT script.c === */

/*! \brief Global flag storage for all scripts. */
extern int8_t g_script_globals[SCRIPT_GLOBALS_MAX];

/**
 * \brief Define extern constants that can be used e.g. in spawners.
 */
#define SCRIPT_CB_TABLE_CONSTS( idx, name, c ) extern const uint16_t SCRIPT_ACTION_ ## name;

SCRIPT_CB_TABLE( SCRIPT_CB_TABLE_CONSTS );

extern const SCRIPT_CB gc_script_handlers[];
#endif /* !SCRIPT_C */

#endif /* SCRIPT_H */

