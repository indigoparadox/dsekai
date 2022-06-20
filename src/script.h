
#ifndef SCRIPT_H
#define SCRIPT_H

/**
 * \addtogroup scripting Scripting
 * \brief Control mobile behavior and interaction.
 *
 * \{
 */

/*! \file script.h
 *  \brief Functions and macros for executing in-world behavior.
 */

struct MOBILE;
struct SCRIPT;
struct TILEMAP;

/*! \brief Maximum number of global script flags available. */
#define SCRIPT_GLOBALS_MAX 20
/*! \brief Maximum depth of available local stack for each ::SCRIPT. */
#define SCRIPT_STACK_DEPTH 10
/*! \brief Value of script arg to indicate real arg should be popped from stack.
 */
#define SCRIPT_ARG_STACK 32767
#define SCRIPT_ARG_RANDOM 32766
/*! \brief Maximum length of a script in text form. */
#define SCRIPT_STR_MAX 1024

/* TODO: This is a valid stack value. */
#define SCRIPT_ERROR_OVERFLOW -1

#ifndef NO_SCRIPT_PROTOTYPES

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

#endif /* !NO_SCRIPT_PROTOTYPES */

#ifndef NO_SCRIPT_STRUCT
#include "scstruct.h"
#endif /* !NO_SCRIPT_STRUCT */

/**
 * \addtogroup scripting_commands_sect Scripting Commands
 * \{
 * \page scripting_commands Scripting Commands
 * 
 * \section scripting_commands_i INTERACT (i)
 *
 * \section scripting_commands_h GIVE (i)
 * Give a "copy" of the item with the GID specified by arg and the owner
 * ::ITEM_OWNER_META to the actee.
 *
 * \}
 */

/**
 * \brief Define the script action callback table.
 * \param f Macro to execute on the function callback definition.
 */
#define SCRIPT_CB_TABLE( f ) f( 0, NOOP, '\0' ) f( 1, INTERACT, 'i' ) f( 2, WALK_NORTH, 'u' ) f( 3, WALK_SOUTH, 'd' ) f( 4, WALK_EAST, 'r' ) f( 5, WALK_WEST, 'l' ) f( 6, SLEEP, 's' ) f( 7, START, 't' ) f( 8, GOTO, 'g' ) f( 9, SPEAK, 'p' ) f( 10, RETURN, 'x' ) f( 11, FACE, 'f' ) f( 12, GLOBAL_SET, 'b' ) f( 13, GLOBAL_GET, 'a' ) f( 14, WARP, 'w' ) f( 15, ANIMATE, 'n' ) f( 16, PUSH, 'v' ) f( 17, POP, '^' ) f( 18, EQUAL_JUMP, '=' ) f( 19, GREATER_JUMP, '>' ) f( 20, LESSER_JUMP, '<' ) f( 21, ADD, '+' ) f( 22, SUBTRACT, '-' ) f( 23, ITEM_GIVE, 'h' ) f( 24, ITEM_TAKE, 'k' ) f( 25, DIE, 'z' )

#ifndef NO_SCRIPT_PROTOTYPES

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
   int8_t script_idx,
   char* script_txt, int16_t script_txt_sz, struct SCRIPT* script
) SECTION_SCRIPT;

/**
 * \brief Script subsystem init function that should be called in main().
 * \return 1 if successful or 0 otherwise.
 */
uint8_t script_init() SECTION_SCRIPT;

/**
 * \brief Script subsystem cleanup function that should be called in main().
 */
void script_shutdown() SECTION_SCRIPT;

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

#endif /* !NO_SCRIPT_PROTOTYPES */

#ifndef NO_SCRIPT_TABLES

#ifdef SCRIPT_C

/* === If we're being called inside script.c === */

int8_t g_script_globals[SCRIPT_GLOBALS_MAX];

#define SCRIPT_CB_TABLE_LIST( idx, name, c ) script_handle_ ## name,

RES_CONST SCRIPT_CB gc_script_handlers[] = {
   SCRIPT_CB_TABLE( SCRIPT_CB_TABLE_LIST )
};

#define SCRIPT_CB_TABLE_CONSTS( idx, name, c ) RES_CONST uint16_t SCRIPT_ACTION_ ## name = idx;

SCRIPT_CB_TABLE( SCRIPT_CB_TABLE_CONSTS );

#else

/* === If we're being called inside anything BUT script.c === */

/*! \brief Global flag storage for all scripts. */
extern int8_t g_script_globals[SCRIPT_GLOBALS_MAX];

/**
 * \brief Define extern constants that can be used e.g. in spawners.
 */
#define SCRIPT_CB_TABLE_CONSTS( idx, name, c ) extern RES_CONST uint16_t SCRIPT_ACTION_ ## name;

SCRIPT_CB_TABLE( SCRIPT_CB_TABLE_CONSTS )

extern RES_CONST SCRIPT_CB gc_script_handlers[];
#endif /* !SCRIPT_C */

#endif /* !NO_SCRIPT_TABLES */

/*! \} */

#endif /* SCRIPT_H */

