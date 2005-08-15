#include "kcm_ggz.h"
#include "kcm_ggz_pane.h"

#include <ktabctl.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kstandarddirs.h>
#include <kaboutdata.h>

#include <qlayout.h>
#include <qdir.h>
#include <qstringlist.h>

#include <dlfcn.h>

#include "config.h"

typedef KCMGGZPane* (*panefunc)(QWidget *parent, const char *name);

KCMGGZ::KCMGGZ(KInstance *instance, QWidget *parent, const char *name)
: KCModule(instance, parent, name)
{
	KStandardDirs d;
	QVBoxLayout *vbox;
	KTabCtl *ctl;
	KCMGGZPane *pane;
	void *handle, *func;
	panefunc init;
	QString error;
	QDir *dir;
	QStringList pathlist;

	ctl = new KTabCtl(this);

	//kcmggzdefault = new KCMGGZDefault(ctl);
	//ctl->addTab(kcmggzdefault, "General settings");
	//connect(kcmggzdefault, SIGNAL(signalChanged()), SLOT(slotChanged()));

	pathlist = d.resourceDirs("module");
	for(QStringList::Iterator it = pathlist.begin(); it != pathlist.end(); it++)
	{
		dir = new QDir((*it), "libkcm_ggz_*.so");
		for(unsigned int i = 0; i < dir->count(); i++)
		{
			if(!error.isEmpty()) error.append("\n");
			handle = dlopen(QString("%1/%2").arg((*it)).arg((*dir)[i].latin1()), RTLD_NOW);
			if(handle)
			{
				func = dlsym(handle, "kcmggz_init");
				if(func)
				{
					init = (panefunc)func;
					pane = (*init)(ctl, 0);
					if(pane)
					{
						panelist.append(pane);
						ctl->addTab(pane, pane->caption());
						connect(pane, SIGNAL(signalChanged()), SLOT(slotChanged()));
					}
					else error.append(i18n("Got a NULL object."));
				}
				else error.append(i18n("This is no KCM_GGZ module."));
			}
			else error.append(dlerror());
		}
	}

	vbox = new QVBoxLayout(this, 5);
	vbox->add(ctl);

	load();

	if(!error.isEmpty()) KMessageBox::error(this, QString(i18n("The KCM_GGZ modules couldn't be loaded!\n(%1)")).arg(error), i18n("Error!"));

	setCaption(i18n("Online Games Configuration"));
}

KCMGGZ::~KCMGGZ()
{
}

void KCMGGZ::load()
{
	for(KCMGGZPane *pane = panelist.first(); pane; pane = panelist.next())
		pane->load();
}

void KCMGGZ::save()
{
	for(KCMGGZPane *pane = panelist.first(); pane; pane = panelist.next())
		pane->save();
}

void KCMGGZ::defaults()
{
}

extern "C"
{
	KCModule *create_ggz(QWidget *parent = NULL, const char *name = NULL)
	{
		KInstance *instance;
		instance = new KInstance("kcmggz");
		return new KCMGGZ(instance, parent, name);
	}
}

void KCMGGZ::slotChanged()
{
	emit changed(true);
}

QString KCMGGZ::quickHelp()
{
	return i18n("<h1>GGZ Gaming Zone</h1> "
		" is a common way for playing internet games. Although it"
		" is possible to set all options from within the applications theirselves,"
		" some default data can be entered here which takes then precedence.<p>"
		" Please take a look at <a href='http://www.ggzgamingzone.org/'>www.ggzgamingzone.org</a>"
		" if you want to learn more.");
}

const KAboutData *KCMGGZ::aboutData()
{
	KAboutData *about;

	about = new KAboutData("kcmggz",
		I18N_NOOP("GGZ Gaming Zone Configuration"),
		QString::null,
		QString::null,
		KAboutData::License_GPL,
		I18N_NOOP("Copyright (C) 2002, 2003 Josef Spillner"));

	about->addAuthor("Josef Spillner", QString::null, "josef@ggzgamingzone.org");

	return about;
}

