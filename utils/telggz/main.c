#include <stdio.h>
#include <ggzcore.h>
#include "chat.h"

int main(int argc, char *argv[])
{
	GGZOptions opt;
	int ret;

	opt.flags = GGZ_OPT_MODULES | GGZ_OPT_PARSER;
	opt.debug_file = NULL;
	opt.debug_levels = 0;

	ret = ggzcore_init(opt);
	if(ret != 0)
	{
		printf("TelGGZ: ERROR! GGZ is unavailable.\n");
		exit(-1);
	}
	printf("TelGGZ: Ready.\n");
	printf("Type '/help' to get the list of available commands.\n");
	fflush(NULL);

	chat_loop();

	return 0;
}

