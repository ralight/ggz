#include "ggzintl.h"

#include "config.h"

#include <stdio.h>

void ggz_intl_init(const char *modulename)
{
#ifdef ENABLE_NLS
	bindtextdomain(modulename, LOCALEDIR);
	textdomain(modulename);
	setlocale(LC_ALL, "");
#endif
}

