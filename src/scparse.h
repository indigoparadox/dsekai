
#ifndef SCPARSE_H
#define SCPARSE_H

/* TODO: Rename functions/macros with scparse. */

#define SCRIPT_CS_NONE      0
#define SCRIPT_CS_COMMENT   1

#define script_char_alpha( c ) (96 < c && 123 > c)

void script_cmp_case( char* token, size_t token_sz );
void script_cmp_action( char* token, size_t token_sz, struct SCRIPT_STEP* s );
int16_t script_arg_special( char* token, size_t token_sz );
void script_reset_token( struct SCRIPT_COMPILE_STATE* s );
void script_parse_src( char c, struct SCRIPT_COMPILE_STATE* s );

#ifdef SCPARSE_C

#define SCRIPT_CB_TABLE_TOKEN( idx, name, c ) #name,

/*! \brief List of valid script action tokens. */
const char* gc_sc_tokens[] = {
   SCRIPT_CB_TABLE( SCRIPT_CB_TABLE_TOKEN )
   NULL
};

#define SCRIPT_CB_TABLE_BYTE( idx, name, c ) c,

const char gc_sc_bytes[] = {
   SCRIPT_CB_TABLE( SCRIPT_CB_TABLE_BYTE )
   '\0'
};

#else

extern const char* gc_sc_tokens[];
extern const char gc_sc_bytes[];

#endif /* SCPARSE_C */

#endif /* !SCPARSE_H */

