#include "kcm_ggz.h"
#include "kcm_ggz_pane.h"

#include <ktabctl.h>
#include <klocale.h>
#include <kmessagebox.h>

#include <qlayout.h>
#include <qdir.h>

#include <dlfcn.h>

typedef KCMGGZPane* (*panefunc)(QWidget *parent = NULL, const char *name = NULL);
#define PATH "/usr/local/kde/lib"

KCMGGZ::KCMGGZ(QWidget *parent, const char *name)
: KCModule(parent, name)
{
	QVBoxLayout *vbox;
	KTabCtl *ctl;
	KCMGGZPane *pane;
	void *handle, *func;
	panefunc init;
	QString error;
	QDir *d;

	ctl = new KTabCtl(this);

	//kcmggzdefault = new KCMGGZDefault(ctl);
	//ctl->addTab(kcmggzdefault, "General settings");
	//connect(kcmggzdefault, SIGNAL(signalChanged()), SLOT(slotChanged()));

	d = new QDir(PATH, "libkcm_ggz_*.so");
	for(int i = 0; i < d->count(); i++)
	{
		if(!error.isEmpty()) error.append("\n");
		handle = dlopen(QString("%1/%2").arg(PATH).arg((*d)[i].latin1()), RTLD_NOW);
		if(handle)
		{
			func = dlsym(handle, "kcmggz_init");
			if(func)
			{
				init = (panefunc)func;
				pane = (*init)(ctl);
				if(pane)
				{
					panelist.append(&pane);
					ctl->addTab(pane, pane->caption());
					connect(pane, SIGNAL(signalChanged()), SLOT(slotChanged()));
				}
				else error.append("Got a NULL object.");
			}
			else error.append("This is no KCM_GGZ module.");
		}
		else error.append(dlerror());
	}

	vbox = new QVBoxLayout(this, 5);
	vbox->add(ctl);

	load();

	if(!error.isEmpty()) KMessageBox::error(this, QString("The KCM_GGZ modules couldn't be loaded!\n(%1)").arg(error), "Error!");

	setCaption("Online Games Configuration");
}

KCMGGZ::~KCMGGZ()
{
}

void KCMGGZ::load()
{
}

void KCMGGZ::save()
{
}

void KCMGGZ::defaults()
{
}

extern "C"
{
	KCModule *create_ggz(QWidget *parent = NULL, const char *name = NULL)
	{
		return new KCMGGZ(parent, name);
	}
}

void KCMGGZ::slotChanged()
{
	emit changed(true);
}

QString KCMGGZ::quickHelp()
{
	return i18n("<h1>GGZ Gaming Zone</1> "
				" is a common way for playing internet games. Although it"
				" is possible to set all options from within the applications theirselves,"
				" some default data can be entered here which takes then precedence.<p>"
				" Please take a look at <a href='http://ggz.sourceforge.net'>ggz.sourceforge.net</a>"
				" if you want to learn more.");
}

