#include "kggzsystem.h"

#include <kmessagebox.h>
#include <kstandarddirs.h>
#include <klocale.h>
#include <kglobal.h>

#include <qdir.h>

void KGGZSystem::ensureInstallation()
{
	KStandardDirs *d;

	d = KGlobal::dirs();
#ifdef PREFIX
	d->addPrefix(PREFIX);
#endif
#ifdef GGZDATADIR
	d->addResourceDir("data", GGZDATADIR);
#endif
	d->addResourceDir("data", QDir::home().path() + "/.ggz/games");
}

void KGGZSystem::checkInstallation(QWidget *parent, QString appname)
{
	KStandardDirs *d;
	QString resource;

	d = KGlobal::dirs();
	resource = d->findResource("data", appname + "/");

	if(resource.isEmpty())
	{
		KMessageBox::error(parent,
			i18n("Data files for this game could not be found. "
				"This means most likely that KDEDIRS is not set correctly."),
			i18n("Incomplete installation"));
	}
}

