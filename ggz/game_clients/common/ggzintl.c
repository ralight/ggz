#include "ggzintl.h"

#include "config.h"

#include <stdio.h>

#include <ggz.h>

void ggz_intl_init(const char *modulename)
{
#ifdef ENABLE_NLS
	bindtextdomain(modulename, LOCALEDIR);
	textdomain(modulename);
	setlocale(LC_ALL, "");
	ggz_debug("main", _("nls enabled: %s"), modulename);
#endif
}

