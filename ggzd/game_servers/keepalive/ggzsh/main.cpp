#ifdef HAVE_CONFIG_H
#  include <config.h>			/* Site-specific config */
#endif

// Include files
#include "ggzsh.h"

// Test the GGZ Scripting Host
int main(int argc, char **argv)
{
	GGZScriptingHost *host;

	host = new GGZScriptingHost();
	host->addScript("keepalive_general");
	host->executeMethod("playercount");
	host->executeMethod("fogofwar");
	host->executeMethod("fogofwar");

	delete host;

	return 0;
}

