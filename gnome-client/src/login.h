#ifndef __LOGIN_H__
#define __LOGIN_H__

#include <ggz.h>
#include <ggzcore.h>

void login_connect_failed(void);
void login_failed(void);
void login_start_session(gchar *host, gint port,
			 gchar *username, gchar *password,
			 GGZLoginType type);
void login_disconnect(void);

#endif

