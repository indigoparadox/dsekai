
#ifndef IRC_H
#define IRC_H

#define IRC_TABLE( f ) f( USER ) f( NICK ) f( QUIT ) f( ISON ) f( JOIN ) f( PART ) f( PRIVMSG ) f( WHO ) f( PING ) f( ERROR )

typedef int16_t (*IRC_HANDLER)(
   struct NETWORK_CONNECTION* n, struct DSEKAI_STATE* state );

#define IRC_TABLE_PROTOTYPES( token ) int16_t irc_handle_ ## token ( struct NETWORK_CONNECTION* n, struct DSEKAI_STATE* state );

IRC_TABLE( IRC_TABLE_PROTOTYPES )

#ifdef IRC_C

#define IRC_TABLE_TOKENS( token ) #token,

const char* gc_irc_tokens[] = {
   IRC_TABLE( IRC_TABLE_TOKENS )
};

#else

#endif /* IRC_C */

#endif /* !IRC_H */

