
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

/**
 * \addtogroup script_args Script Instruction Arguments
 * \brief Immediate arguments for script instructions.
 * \{
 */

/**
 * \brief Next instruction should pop its arg from stack, and then push
 *        it back when it's done.
 */
#define SCRIPT_ARG_STACK_P 32763

/**
 * \brief Next instruction should pop its arg from stack, and then call
 *        ::mobile_incr_icount to increment interaction count.
 */
#define SCRIPT_ARG_STACK_I 32767

/**
 * \brief Next instruction should pop its arg from stack.
 */
#define SCRIPT_ARG_STACK 32765
#define SCRIPT_ARG_RANDOM 32766
#define SCRIPT_ARG_FOLLOW 32764

/**
 * \brief The maximum script argument guaranteed to be taken literally.
 *        Numbers above this may be interpreted as special instructions.
 */
#define SCRIPT_ARG_MAX 32750

/*! \} */

/*! \brief Maximum number of global script flags available. */
#define SCRIPT_GLOBALS_MAX 20
/*! \brief Maximum depth of available local stack for each ::SCRIPT. */
#define SCRIPT_STACK_DEPTH 10
/*! \brief Maximum length of a script in text form. */
#define SCRIPT_STR_MAX 1024

#define SCRIPT_STACK_MAX 127

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
 * \addtogroup scripting_instructions_sect Scripting Instructions
 * \brief Overview of the mobile scripting language.
 *
 * All statements take a single numerical argument.
 * 
 * Args can range from 0 to ::SCRIPT_ARG_MAX. See \ref scripting_args for more
 * information on special arguments.
 *
 * Items on the stack can range from 0 to ::SCRIPT_STACK_MAX.
 * \{
 */

#define SCRIPT_CB_TABLE_23( f ) f( 23, DISABLE, 'd' )

/**
 * \brief \b WARP: Warp to a tilemap identified by the string at the arg index
 *        in current TILEMAP::strpool.
 *
 * \b Arguments
 * - Index of a string containing the name of the ::TILEMAP to warp to in
 *   the currently loaded TILEMAP::strpool.
 */
#define SCRIPT_CB_TABLE_22( f ) f( 22, WARP,    'w' ) SCRIPT_CB_TABLE_23( f )
#define SCRIPT_CB_TABLE_21( f ) f( 21, ANIM,    'n' ) SCRIPT_CB_TABLE_22( f )
#define SCRIPT_CB_TABLE_20( f ) f( 20, PUSH,    'v' ) SCRIPT_CB_TABLE_21( f )
#define SCRIPT_CB_TABLE_19( f ) f( 19, POP,     '^' ) SCRIPT_CB_TABLE_20( f )

/**
 * \brief \b EQJMP: Move script execution to a label defined by \b START if
 *        the last value pushed to the stack is \b EQUAL \b TO the value
 *        pushed to the stack before it.
 *
 * \b Arguments
 * - The index of the label as defined by a \b START instruction in the same
 *   script to jump to if the condition is \b TRUE.
 *
 * \b Stack \b Values \b Pushed
 * - MOBILE::script_pc of next instruction to execute otherwise.
 *
 * \attention If (and only if) the condition is true and the jump is performed,
 *            this instruction will automatically push the index of the next
 *            instruction that would have otherwise been executed to the
 *            stack, so that it may be returned to with \b RETURN and 
 *            e.g. ::SCRIPT_ARG_STACK.
 */
#define SCRIPT_CB_TABLE_18( f ) f( 18, EQJMP,   '=' ) SCRIPT_CB_TABLE_19( f )

/**
 * \brief \b GTJMP: Move script execution to a label defined by \b START if
 *        the last value pushed to the stack is \b GREATER \b THAN the value
 *        pushed to the stack before it.
 *
 * \b Arguments
 * - The index of the label as defined by a \b START instruction in the same
 *   script to jump to if the condition is \b TRUE.
 *
 * \b Stack \b Values \b Pushed
 * - MOBILE::script_pc of next instruction to execute otherwise.
 *
 * \attention If (and only if) the condition is true and the jump is performed,
 *            this instruction will automatically push the index of the next
 *            instruction that would have otherwise been executed to the
 *            stack, so that it may be returned to with \b RETURN and 
 *            e.g. ::SCRIPT_ARG_STACK.
 */
#define SCRIPT_CB_TABLE_17( f ) f( 17, GTJMP,   '>' ) SCRIPT_CB_TABLE_18( f )

/**
 * \brief \b LTJMP: Move script execution to a label defined by \b START if
 *        the last value pushed to the stack is \b LESS \b THAN the value
 *        pushed to the stack before it.
 *
 * \b Arguments
 * - The index of the label as defined by a \b START instruction in the same
 *   script to jump to if the condition is \b TRUE.
 *
 * \b Stack \b Values \b Pushed
 * - MOBILE::script_pc of next instruction to execute otherwise.
 *
 * \attention If (and only if) the condition is true and the jump is performed,
 *            this instruction will automatically push the index of the next
 *            instruction that would have otherwise been executed to the
 *            stack, so that it may be returned to with \b RETURN and 
 *            e.g. ::SCRIPT_ARG_STACK.
 */
#define SCRIPT_CB_TABLE_16( f ) f( 16, LTJMP,   '<' ) SCRIPT_CB_TABLE_17( f )

/**
 * \brief \b ADD: Add the argument to the last value pushed to the stack,
 *        and push the result back onto the stack.
 *
 * \b Stack \b Values \b Popped
 * - Value to add.
 * - Value to add to.
 *
 * \b Stack \b Values \b Pushed
 * - Subtraction result.
 */
#define SCRIPT_CB_TABLE_15( f ) f( 15, ADD,     '+' ) SCRIPT_CB_TABLE_16( f )

/**
 * \brief \b SUB: Subtract the argument from the last value pushed to the
 *        stack, and push the result back onto the stack.
 *
 * \b Arguments
 * - Value to subtract.
 *
 * \b Stack \b Values \b Popped
 * - Value to subtract from.
 *
 * \b Stack \b Values \b Pushed
 * - Subtraction result.
 */
#define SCRIPT_CB_TABLE_14( f ) f( 14, SUB,     '-' ) SCRIPT_CB_TABLE_15( f )

/*
 * \brief \b GIVE: Give an ::ITEM to DSEKAI_STATE::player based on ITEM::gid
 *        from TILEMAP::item_defs.
 *
 * \b Arguments
 * - ITEM::gid to give from TILEMAP::item_defs.
 *
 * \todo TODO: Push number given to stack if successful.
 */
#define SCRIPT_CB_TABLE_13( f ) f( 13, GIVE,    'h' ) SCRIPT_CB_TABLE_14( f )

/**
 * \brief \b TAKE: Give an ::ITEM to DSEKAI_STATE::player based on ITEM::gid
 *        from TILEMAP::item_defs.
 *
 * \b Arguments
 * - ITEM::gid to take from player inventory.
 *
 * \attention: If the ::MOBILE executing this script is ::MOBILE_TYPE_SPECIAL,
 *             the item will remain in this mobile's inventory forever,
 *             wasting the limited resources of the engine. Try to only take
 *             if the item will be given back or if the mobile is disposable.
 *
 * \todo TODO: Push number taken to stack if successful.
 */
#define SCRIPT_CB_TABLE_12( f ) f( 12, TAKE,    'k' ) SCRIPT_CB_TABLE_13( f ) 

/**
 * \brief \b DIE: Trigger death animation for ::MOBILE running this script
 *        and then vanish, becoming inactive.
 *
 * See \ref dsekai_mobiles_mp_hp_sect for more information on mobile death.
 */
#define SCRIPT_CB_TABLE_11( f ) f( 11, DIE,     'z' ) SCRIPT_CB_TABLE_12( f )

/**
 * \brief \b GGET: Push a value from the global table at onto stack.
 *
 * \b Arguments
 * - Index of the value to push in ::g_script_globals.
 *
 * \b Stack \b Values \b Pushed
 * - Value of ::g_script_globals at the index specified.
 */
#define SCRIPT_CB_TABLE_10( f ) f( 10, GGET,    'a' ) SCRIPT_CB_TABLE_11( f )

/**
 * \brief \b GSET: Pop a value from the stack into the global table.
 *
 * \b Arguments
 * - Index of the value to pop to in ::g_script_globals.
 *
 * \b Stack \b Values \b Popped
 * - Value to insert into ::g_script_globals at the index specified.
 */
#define SCRIPT_CB_TABLE_9( f ) f( 9,   GSET,    'b' ) SCRIPT_CB_TABLE_10( f )

/**
 * \brief \b FACE: Set the actor face one of the
 *        \ref dsekai_mobiles_directions.
 *
 * \b Arguments
 * - One of the \ref dsekai_mobiles_directions.
 *
 * \note If the argument supplied is greater than 3, it will be divided by
 *       4 and the remainder will be interpreted as the direction to face.
 */
#define SCRIPT_CB_TABLE_8( f ) f( 8,   FACE,    'f' ) SCRIPT_CB_TABLE_9( f )

/**
 * \brief \b RETURN: Return from an interaction subroutine to the main script.
 *
 * \b Arguments
 * - The instruction index to return to.
 * 
 * It is common to use this with ::SCRIPT_ARG_STACK_I, which uses the
 * instruction number automatically pushed to the stack on interaction, and
 * then increments the interaction count. This is not necessary, however, and
 * more creative uses may be devised.
 *
 * \note This instruction returns to an individual instruction index in the
 *       sequence of the script by setting MOBILE::script_pc directly,
 *       \b NOT by a label defined by \b START.
 */
#define SCRIPT_CB_TABLE_7( f ) f( 7,   RETURN,  'x' ) SCRIPT_CB_TABLE_8( f )

/**
 * \brief \b SPEAK: Display a dialog window with the actor's MOBILE::sprite_id
 *        and a line of text from TILEMAP::strpool.
 *
 * \b Arguments
 * - String index in TILEMAP::strpool to display.
 */
#define SCRIPT_CB_TABLE_6( f ) f( 6,   SPEAK,   'p' ) SCRIPT_CB_TABLE_7( f )

/**
 * \brief \b GOTO: Move script execution to a label defined by \b START.
 *
 * \b Arguments
 * - The index of the label as defined by a \b START instruction in the same
 *   script to jump to.
 *
 * \b Stack \b Values \b Pushed
 * - MOBILE::script_pc of next instruction to execute otherwise.
 *
 * \attention This instruction will automatically push the index of the next
 *            instruction that would have otherwise been executed to the
 *            stack, so that it may be returned to with \b RETURN and 
 *            e.g. ::SCRIPT_ARG_STACK.
 */
#define SCRIPT_CB_TABLE_5( f ) f( 5,   GOTO,    'g' ) SCRIPT_CB_TABLE_6( f )

/**
 * \brief \b START: Define a label index that can be jumped to.
 *
 * \b Arguments
 * - Index number for this label.
 *
 * Labelled indexes defined with \b START may be jumped to later by \b GOTO
 * or \b JMP instructions.
 */
#define SCRIPT_CB_TABLE_4( f ) f( 4,   START,   't' ) SCRIPT_CB_TABLE_5( f )

/**
 * \brief \b SLEEP: Don't do anything for \a arg number of frames.
 *
 * \b Arguments
 * - Number of frames to sleep. This may vary in terms of precise timing by
 *   platform.
 */
#define SCRIPT_CB_TABLE_3( f ) f( 3,   SLEEP,   's' ) SCRIPT_CB_TABLE_4( f )

/**
 * \brief \b WALK: The actor walks in the direction specified on the stack.
 *
 * \b Arguments
 * - Number of steps to walk.
 *
 * \b Stack Values Popped
 * - One of the \ref dsekai_mobiles_directions.
 *
 * \note If the direction popped is greater than 3, it will be divided by
 *       4 and the remainder will be interpreted as the direction to walk.
 */
#define SCRIPT_CB_TABLE_2( f ) f( 2,   WALK,    'u' ) SCRIPT_CB_TABLE_3( f )

/**
 * \brief \b INTERACT: Define an interaction callback label.
 *
 * \b Arguments
 * - Index of the interaction callback label to define.
 *
 * This statement does nothing by itself, but it defines a point in the code
 * that will be jumped to when a player interacts with the ::MOBILE running
 * this ::SCRIPT.
 *
 * The index of the interaction callback that will be called is determined
 * by the ::MOBILE_ICOUNT_MASK portion of MOBILE::flags, and may be from
 * 0 through 9. This allows a mobile to "say" one thing on first interaction
 * and then different things on subsequent interactions.
 *
 * \attention On interaction, the script execution engine will automatically
 *            push the index of the next instruction that would have otherwise
 *            been executed to the stack, so that it may be returned to with
 *            \b RETURN and ::SCRIPT_ARG_STACK or ::SCRIPT_ARG_STACK_I.
 */
#define SCRIPT_CB_TABLE_1( f ) f( 1,   INTERACT,'i' ) SCRIPT_CB_TABLE_2( f )

/**
 * \brief \b NOOP: Not a valid instruction. Beginning of the script callback
 *        table, used to implement script parsing.
 */
#define SCRIPT_CB_TABLE( f ) f( 0, NOOP, '\0' ) SCRIPT_CB_TABLE_1( f )

/*! \} */

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

