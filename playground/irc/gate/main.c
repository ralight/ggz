/*******************************************************************
*
* ggz-ircgate - Gateway between GGZ room chat and IRC channel
* Copyright (C) 2008 Josef Spillner <josef@ggzgamingzone.org>
* Based on Grubby chatbot sources
* Published under GNU GPL conditions - see 'COPYING' for details
*
********************************************************************/

#include "net.h"
#include "config.h"
#include <ggz.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>

/* Main: startup until working state, loop */
int main(int argc, char *argv[])
{
	/* Recognize command line arguments */
	struct option options[] =
	{
		{"ircuri", required_argument, 0, 'i'},
		{"ggzuri", required_argument, 0, 'g'},
		{0, 0, 0, 0}
	};
	int optindex;
	int opt;
	const char *ircuri = NULL;
	const char *ggzuri = NULL;
	char *msg;

	while(1)
	{
		opt = getopt_long(argc, argv, "i:g:", options, &optindex);
		if(opt == -1) break;
		switch(opt)
		{
			case 'i':
				ircuri = optarg;
				break;
			case 'g':
				ggzuri = optarg;
				break;
			default:
				printf("Syntax: ggz-ircgate --ggzuri <uri> --ircuri <uri>\n");
				exit(-1);
				break;
		}
	}

	/* Start connection procedure */
	net_ggz_connect(ggzuri);
	net_irc_connect(ircuri);

	/* Main loop */
	while(1)
	{
		/* Permanently check states */
		switch(net_ggz_status())
		{
			case NET_ERROR:
				printf("Error on GGZ!");
				exit(-1);
				break;
			case NET_INPUT:
				msg = net_ggz_input();
				net_irc_output(msg);
				ggz_free(msg);
				break;
		}

		switch(net_irc_status())
		{
			case NET_ERROR:
				printf("Error on IRC!");
				exit(-1);
				break;
			case NET_INPUT:
				msg = net_irc_input();
				net_ggz_output(msg);
				ggz_free(msg);
				break;
		}
	}

	/* Shutdown the bot properly */
	net_ggz_cleanup();
	net_irc_cleanup();

	return 0;
}

