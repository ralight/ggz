#include "ggzintl.h"

#include <stdio.h>

#include <ggz.h>

void ggz_intl_init(const char *modulename)
{
#ifdef ENABLE_NLS
	bindtextdomain(modulename, NULL);
	textdomain(modulename);
	setlocale(LC_ALL, "");
	ggz_debug("main", _("nls enabled: %s"), modulename);
#endif
}

