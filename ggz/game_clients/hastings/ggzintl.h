#ifndef GGZ_INTL_H
#define GGZ_INTL_H

/* Useful functions to be used in projects with more than one binary/package. */
/* Call ggz_intl_init("foo") first, and do a "touch ../po/foo/de.po" or similar. */

#ifdef ENABLE_NLS
#  include <locale.h>
#  include <libintl.h>
#  define _(x) gettext(x)
#else
#  define _(x) x
#endif

#define N_(x) x

void ggz_intl_init(const char *modulename);

#endif

