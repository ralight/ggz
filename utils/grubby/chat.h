#ifndef _AGRUB_CHAT_
#define _AGRUB_CHAT_

void handle_chat( char  OpCode, char *from, char *message );
void handle_join( char *from );
void handle_part( char *from );
void send_msg( char *to, char *message );
void send_chat( char *message );
void send_chat_insert_name( char *from, char *msg );

#endif
