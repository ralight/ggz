#ifndef NET_H
#define NET_H

#include <ggzcore.h>

void net_init();
void net_login(char *username, char *password);
void net_process();
void net_send(char *buffer);
void net_allow(int allow);
void net_join(int roomnum);
void net_host(char *hostname);

GGZHookReturn net_hook_connect(unsigned int id, void *event_data, void *user_data);
GGZHookReturn net_hook_failure(unsigned int id, void *event_data, void *user_data);
GGZHookReturn net_hook_login(unsigned int id, void *event_data, void *user_data);
GGZHookReturn net_hook_roomlist(unsigned int id, void *event_data, void *user_data);
GGZHookReturn net_hook_enter(unsigned int id, void *event_data, void *user_data);
GGZHookReturn net_hook_roomenter(unsigned int id, void *event_data, void *user_data);
GGZHookReturn net_hook_roomleave(unsigned int id, void *event_data, void *user_data);
GGZHookReturn net_hook_chat(unsigned int id, void *event_data, void *user_data);
GGZHookReturn net_hook_prvmsg(unsigned int id, void *event_data, void *user_data);

#endif

