
#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#define RES_CONST const
#define NO_SCRIPT_STRUCT
#define NO_SCRIPT_PROTOTYPES
#define NO_SCRIPT_TABLES

#include "../src/script.h"

#define SCRIPT_CB_TABLE_TOKEN( idx, name, c ) #name,

const char* gc_sc_tokens[] = {
   SCRIPT_CB_TABLE( SCRIPT_CB_TABLE_TOKEN )
   NULL
};

#define SCRIPT_CB_TABLE_BYTE( idx, name, c ) c,

const char gc_sc_bytes[] = {
   SCRIPT_CB_TABLE( SCRIPT_CB_TABLE_BYTE )
   '\0'
};

#define CS_INST      0
#define CS_PARM      1

#define TOKEN_ITER_SZ_MAX 255
#define INST_SZ_MAX 255

#define char_alpha( c ) (96 < c && 123 > c)

#define d_printf( ... ) printf( __VA_ARGS__ )

struct COMPILE_INST {
   uint16_t op;
   int16_t parm;
   char label[TOKEN_ITER_SZ_MAX + 1];
};

struct COMPILE_STATE {
   uint8_t state;
   char token_iter[TOKEN_ITER_SZ_MAX + 1];
   size_t token_iter_sz;
   struct COMPILE_INST inst[INST_SZ_MAX + 1];
   size_t inst_sz;
   int16_t last_label;
};

void compare_case( char* token, size_t token_sz ) {
   uint16_t i = 0;

   /* Change token case for compare. */
   for( i = 0 ; token_sz > i ; i++ ) {
      if( char_alpha( token[i] ) ) {
         token[i] -= 32;
      }
   }
}

void compare_inst( char* token, size_t token_sz, struct COMPILE_INST* s ) {
   uint16_t i = 0;

   compare_case( token, token_sz );

   /* Compare to token table. */
   i = 0;
   while( NULL != gc_sc_tokens[i] ) {
      if( 0 == strncmp( token, gc_sc_tokens[i], token_sz ) ) {
         s->op = i;
         d_printf( "inst: %s\n", gc_sc_tokens[i] );
         break;
      }
      i++;
   }
}

int16_t parm_dir( char* token, size_t token_sz ) {

   compare_case( token, token_sz );

   if( 0 == strncmp( token, "SOUTH", token_sz ) ) {
      return 0;
   } else if( 0 == strncmp( token, "NORTH", token_sz ) ) {
      return 1;
   } else if( 0 == strncmp( token, "EAST", token_sz ) ) {
      return 2;
   } else if( 0 == strncmp( token, "WEST", token_sz ) ) {
      return 3;
   }

   return -1;   
}

int16_t parm_label(
   char* token, size_t token_sz, struct COMPILE_INST* labels, size_t labels_sz
) {
   int16_t i = 0;

   compare_case( token, token_sz );

   for( i =  0 ; labels_sz > i ; i++ ) {
      if( 0 == strncmp( token, labels[i].label, token_sz ) ) {
         return i;
      }
   }

   return -1;   
}

void reset_token( struct COMPILE_STATE* s ) {
   s->token_iter_sz = 0;
   s->token_iter[s->token_iter_sz] = '\0';
}

void parse( char c, struct COMPILE_STATE* s ) {
   int16_t parm_tmp = 0;
   
   switch( c ) {

   case ':':
      strncpy( s->inst[s->inst_sz].label, s->token_iter, s->token_iter_sz );
      compare_case( s->inst[s->inst_sz].label, s->token_iter_sz );
      s->inst[s->inst_sz].op = 7; /* START */
      s->inst[s->inst_sz].parm = s->last_label++;
      d_printf( "l: %s\n", s->inst[s->inst_sz].label );
      s->inst_sz++;
      reset_token( s );
      break;

   case '\r':
   case '\n':
   case ' ':
      /* Do nothing if token buffer empty. */
      if( 0 == s->token_iter_sz ) {
         break;
      }

      /* Process token. */
      d_printf( "t: %s\n", s->token_iter );
      if( 0 < s->inst[s->inst_sz].op ) {
         /* Set parm and advance instruction. */
         if( 11 == s->inst[s->inst_sz].op ) {
            /* FACE */
            parm_tmp = parm_dir( s->token_iter, s->token_iter_sz );
            assert( 0 <= parm_tmp );
         } else if( 8 == s->inst[s->inst_sz].op ) {
            /* GOTO */
            parm_tmp = parm_label(
               s->token_iter, s->token_iter_sz, s->inst, s->inst_sz );
            assert( 0 <= parm_tmp );
         } else {
            parm_tmp = atoi( s->token_iter );
         }
         s->inst[s->inst_sz].parm = parm_tmp;
         d_printf( "parm: %d\n", s->inst[s->inst_sz].parm );
         s->inst_sz++;
      } else {
         compare_inst(
            s->token_iter, s->token_iter_sz, &(s->inst[s->inst_sz]) );
      }
      reset_token( s );
      break;

   default:
      /* Reset token buffer. */
      s->token_iter[s->token_iter_sz++] = c;
      s->token_iter[s->token_iter_sz] = '\0';
      break;
   }
}

int main( int argc, char** argv ) {
   int retval = 0;
   FILE* in_file = NULL;
   size_t in_sz = 0,
      in_read = 0,
      i = 0;
   uint8_t* in_bytes = NULL;
   struct COMPILE_STATE s;

   assert( 2 == argc );

   /* Open file. */
   in_file = fopen( argv[1], "r" );
   assert( NULL != in_file );

   /* Get size. */
   fseek( in_file, 0, SEEK_END );
   in_sz = ftell( in_file );
   fseek( in_file, 0, SEEK_SET );

   /* Allocate, read. */
   in_bytes = calloc( in_sz, 1 );
   assert( NULL != in_bytes );

   in_read = fread( in_bytes, 1, in_sz, in_file );
   assert( in_read == in_sz );

   /* Parse. */
   memset( &s, '\0', sizeof( struct COMPILE_STATE ) );

   for( i = 0 ; in_sz > i ; i++ ) {
      parse( in_bytes[i], &s );
   }

   /* Add final RETURN. */
   s.inst[s.inst_sz].op = 10;
   s.inst[s.inst_sz].parm = 32767;
   s.inst_sz++;

   /* Display bytes. */
   d_printf( "\n" );
   for( i = 0 ; s.inst_sz > i ; i++ ) {
      printf( "%c%d", gc_sc_bytes[s.inst[i].op], s.inst[i].parm );
   }
   printf( "\n" );

cleanup:

   fclose( in_file );

   return retval;
}

