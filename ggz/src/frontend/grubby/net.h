#ifndef _AGRUB_NET_
#define _AGRUB_NET_

#include <ggzcore.h>

void get_op_code( void );

void net_send_msg(char *message);
void net_send_prvmsg(char *from, char *message);
void net_send_chat_insert_name(char *from, char *msg);
void net_change_room(int room_num);
void net_connect(void);
void net_register_hooks(void);

#endif
