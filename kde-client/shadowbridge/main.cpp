#include "shadowapp.h"

#include <kapp.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <klocale.h>

int main(int argc, char **argv)
{
	KApplication *a;
	ShadowApp *app;
	KAboutData *aboutData;
	KCmdLineArgs *args;

	aboutData = new KAboutData("shadowbridge",
		I18N_NOOP("GGZ Shadow Bridge"),
		"0.1",
		I18N_NOOP("Drop-in fake game client to record\nand distribute game events."),
		KAboutData::License_GPL,
		"(C) 2001 Josef Spillner\n"
		"The MindX Open Source Project",
		I18N_NOOP("ShadowBridge"),
		"http://ggz.sourceforge.net",
		"dr_maux@users.sourceforge.net");

	aboutData->addAuthor("Josef Spillner", I18N_NOOP("Author"), "dr_maux@users.sourceforge.net", "http://mindx.sourceforge.net");

	KCmdLineArgs::init(argc, argv, aboutData);

	a = new KApplication();
	app = new ShadowApp();
	a->setMainWidget(app);
	return a->exec();
}

