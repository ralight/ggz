#include <stdio.h>
#include <ggzcore.h>
#include <string.h>
#include <unistd.h>
#include "chat.h"
#include "meta.h"

/* TelGGZ version number */
#define VERSION "0.1"

/* Structure containing server information */
ServerGGZ **preflist;

/* Print out all servers to play wrapper for */
void pref_listservers()
{
	ServerGGZ *iterator;
	int i;

	/* Of course I meant, people will kill me for C++isms... */
	i = 0;
	while((preflist) && (preflist[i]))
	{
		iterator = preflist[i++];
		printf("%3i %22s %10s %20s %10ik\n", iterator->id, iterator->host,
			iterator->version, iterator->location, iterator->speed);
	}
}

/* The main function. How sweet. */
int main(int argc, char *argv[])
{
	GGZOptions opt;
	int ret;
	char hostname[128];

	/* Meta server stuff */
	meta_init();
	meta_sync();
	preflist = meta_query("0.0.5pre");
	/*meta_free(preflist);*/

	/* This could be the default values. But we code in C here. */
	opt.flags = GGZ_OPT_MODULES | GGZ_OPT_PARSER;
	opt.debug_file = NULL;
	opt.debug_levels = 0;

	/* We just announce us the the world. */
	if(gethostname(hostname, sizeof(hostname))) strcpy(hostname, "[unknown]");
	printf("[TelGGZ: GGZ Gaming Zone Telnet Wrapper v%s]\n", VERSION);
	printf("You're connected to %s.\n", hostname);
	fflush(NULL);

	/* Make sure GGZ is actually available. */
	ret = ggzcore_init(opt);
	if(ret != 0)
	{
		printf("TelGGZ: Error! GGZ is currently unavailable. Please join back later.\n");
		fflush(NULL);
		exit(-1);
	}
	/*printf("Type '/help' to get the list of available commands.\n");
	fflush(NULL);*/

	/* Let the chatter select a server. */
	printf("Please select a server from the list below.\n\n");
	pref_listservers();
	printf("\n");
	printf("Your choice: ");
	fflush(NULL);
	ret = chat_getserver() - 1;

	/* Connect to that server */
	if(ret >= 0)
	{
		printf("Connecting to %s:%i...\n", preflist[ret]->host, preflist[ret]->port);
		fflush(NULL);
		chat_connect(preflist[ret]->host, preflist[ret]->port);
		//printf("ok");
		//fflush(NULL);

		/* Enter the chat loop */
		chat_loop();
	}

	/* Exit with humor */
	return 0;
}

