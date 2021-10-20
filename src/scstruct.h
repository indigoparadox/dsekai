
#ifndef SCSTRUCT_H
#define SCSTRUCT_H

/*! \brief Maximum number of steps in an individual script. */
#define SCRIPT_STEPS_MAX 16

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

#endif /* !SCSTRUCT_H */

