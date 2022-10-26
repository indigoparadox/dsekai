
#ifndef SCSTRUCT_H
#define SCSTRUCT_H

/**
 * \addtogroup scripting
 *
 * \{
 */

/*! \file scstruct.h
 *  \brief Structs representing behavior scripts.
 */

/**
 * \brief Contains a single instruction in a script, in SCRIPT::steps.
 *
 *  Modifications to this struct should also be reflected in
 *  tools/map2h.c (map2h.h).
 */
struct SCRIPT_STEP {
   /*! \brief Opcode of the action as defined by ::SCRIPT_CB_TABLE. */
   uint16_t action;
   /*! \brief Argument that will be passed to this action on execution. */
   int16_t arg;
};

/**
 * \brief Contains immutable state for scripts attached to a TILEMAP.
 *
 *  Modifications to this struct should also be reflected in
 *  tools/map2h.c (map2h.h).
 */
struct SCRIPT {
   /*! \brief Indexed list of script steps in order. */
   struct SCRIPT_STEP steps[SCRIPT_STEPS_MAX];
   /*! \brief Number of SCRIPT::steps attached to this script. */
   uint16_t steps_count;
};

#ifndef NO_SCRIPT_COMPILER

/**
 * \addtogroup scripting_compiler Script Compiler
 */

#define SCRIPT_TOKEN_ITER_SZ_MAX 255

struct SCRIPT_COMPILE_STATE {
   uint8_t flags;
   char token_iter[SCRIPT_TOKEN_ITER_SZ_MAX + 1];
   uint16_t token_iter_sz;
   struct SCRIPT_STEP* steps;
   uint16_t steps_sz;
   int16_t last_start;
};

#endif /* !NO_SCRIPT_COMPILER */

/*! \} */

#endif /* !SCSTRUCT_H */

