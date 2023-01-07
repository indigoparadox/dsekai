
#define SCPARSE_C
#include "dsekai.h"

/* For atoi. */
#include <stdlib.h>

void script_cmp_case( char* token, size_t token_sz ) {
   uint16_t i = 0;

   /* Change token case for compare. */
   for( i = 0 ; token_sz > i ; i++ ) {
      if( script_char_alpha( token[i] ) ) {
         token[i] -= 32;
      }
   }
}

void script_cmp_action( char* token, size_t token_sz, struct SCRIPT_STEP* s ) {
   uint16_t i = 0;

   script_cmp_case( token, token_sz );

   /* Compare to token table. */
   i = 0;
   while( NULL != gc_sc_tokens[i] ) {
      if( 0 == memory_strncmp_ptr( token, gc_sc_tokens[i], token_sz ) ) {
         s->action = i;
         script_trace_printf( 1, " action: %s\n", gc_sc_tokens[i] );
         return;
      }
      i++;
   }

   s->action = 0;
}

int16_t script_arg_special( char* token, size_t token_sz ) {

   script_cmp_case( token, token_sz );

   if( 0 == memory_strncmp_ptr(        "SOUTH",   token, 5 ) ) {
      return 0;
   } else if( 0 == memory_strncmp_ptr( "NORTH",   token, 5 ) ) {
      return 1;
   } else if( 0 == memory_strncmp_ptr( "EAST",    token, 4 ) ) {
      return 2;
   } else if( 0 == memory_strncmp_ptr( "WEST",    token, 4 ) ) {
      return 3;
   } else if( 0 == memory_strncmp_ptr( "STACK_I", token, 7 ) ) {
      return SCRIPT_ARG_STACK_I;
   } else if( 0 == memory_strncmp_ptr( "STACK_P", token, 7 ) ) {
      return SCRIPT_ARG_STACK_P;
   } else if( 0 == memory_strncmp_ptr( "STACK",   token, 5 ) ) {
      return SCRIPT_ARG_STACK;
   } else if( 0 == memory_strncmp_ptr( "RANDOM",  token, 6 ) ) {
      return SCRIPT_ARG_RANDOM;
   } else if( 0 == memory_strncmp_ptr( "FOLLOW",  token, 6 ) ) {
      return SCRIPT_ARG_FOLLOW;
   }

   return -1;   
}

void script_reset_token( struct SCRIPT_COMPILE_STATE* s ) {
   s->token_iter_sz = 0;
   s->token_iter[s->token_iter_sz] = '\0';
}

void script_parse_src( char c, struct SCRIPT_COMPILE_STATE* s ) {
   int16_t arg_tmp = 0;
   
   switch( c ) {

   case ':':
      if( SCRIPT_CS_COMMENT == (SCRIPT_CS_COMMENT & s->flags) ) {
         /* Colons happen in comments sometimes. */
         break;
      }
      s->steps[s->steps_sz].arg = atoi( s->token_iter );
         
      script_trace_printf( 1, " action: %d, l: %d\n",
         s->steps[s->steps_sz].action, s->steps[s->steps_sz].arg );

      /* Make sure this is an instruction that can be a label. */
      assert(
         SCRIPT_ACTION_START == s->steps[s->steps_sz].action ||
         SCRIPT_ACTION_INTERACT == s->steps[s->steps_sz].action );

      if( 7 == s->steps[s->steps_sz].action ) {
         /* Only increment start count if this is a START. */
         /* (Multiple INTERACTs don't make sense. */
         s->steps[s->steps_sz].arg = s->last_start++;
      }

      s->steps_sz++;
      script_reset_token( s );
      break;

   case '#':
   case ';':
      s->flags |= SCRIPT_CS_COMMENT;
      break;

   case '\r':
   case '\n':
      if( SCRIPT_CS_COMMENT == (SCRIPT_CS_COMMENT & s->flags) ) {
         /* Get out of comment in new line. */
         s->flags &= ~SCRIPT_CS_COMMENT;
         script_reset_token( s );
         break;
      }

   case ' ':
      if( SCRIPT_CS_COMMENT == (SCRIPT_CS_COMMENT & s->flags) ) {
         script_reset_token( s );
         break;
      }

      /* Do nothing if token buffer empty. */
      if( 0 == s->token_iter_sz ) {
         break;
      }

      /* Process token. */
      script_trace_printf( 1, "token #%d: %s\n", s->steps_sz, s->token_iter );
      if( 0 < s->steps[s->steps_sz].action ) {
         /* Set arg and advance instruction. */
         
         arg_tmp = script_arg_special( s->token_iter, s->token_iter_sz );

         /* All special args are positive, so negative means failure. */
         if( 0 > arg_tmp ) {
            arg_tmp = atoi( s->token_iter );
         }
         s->steps[s->steps_sz].arg = arg_tmp;
         script_trace_printf( 1, " arg: %d\n", s->steps[s->steps_sz].arg );
         s->steps_sz++;
      
      } else {
         /* Read instruction. */
         script_cmp_action(
            s->token_iter, s->token_iter_sz, &(s->steps[s->steps_sz]) );
      }
      script_reset_token( s );
      break;

   default:
      if( SCRIPT_CS_COMMENT == (SCRIPT_CS_COMMENT & s->flags) ) {
         break;
      }
      /* Reset token buffer. */
      s->token_iter[s->token_iter_sz++] = c;
      s->token_iter[s->token_iter_sz] = '\0';
      break;
   }
}
