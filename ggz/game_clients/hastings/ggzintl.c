#include "ggzintl.h"

#include <stdio.h>

void ggz_intl_init(const char *modulename)
{
#ifdef ENABLE_NLS
	bindtextdomain(modulename, NULL);
	textdomain(modulename);
	setlocale(LC_ALL, "");
	printf(_("nls enabled: %s\n"), modulename);
#endif
}

