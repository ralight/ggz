#include "prefs.h"

static Prefs *s_prefs = NULL;

Prefs::Prefs()
{
	s_prefs = this;

	setCurrentGroup("Settings");
	addItemBool("sync", sync);
}

Prefs *Prefs::self()
{
	if(s_prefs)
		return s_prefs;
	return new Prefs();
}

