/*******************************************************************
*
* ggz-ircgate - Gateway between GGZ room chat and IRC channel
* Copyright (C) 2008 Josef Spillner <josef@ggzgamingzone.org>
* Based on Grubby chatbot sources
* Published under GNU GPL conditions - see 'COPYING' for details
*
********************************************************************/

#ifndef GGZ_IRCGATE_NET_H
#define GGZ_IRCGATE_NET_H

/* Available generic states of the net module */
enum States
{
	NET_NOOP,
	NET_ERROR,
	NET_INPUT
};

/* Interface */
void net_ggz_cleanup(void);
void net_ggz_connect(const char *ggz_uri);
int net_ggz_status(void);
char *net_ggz_input(void);
void net_ggz_output(const char *output);

void net_irc_cleanup(void);
void net_irc_connect(const char *irc_uri);
int net_irc_status(void);
char *net_irc_input(void);
void net_irc_output(const char *output);

#endif

