#include <kapp.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <klocale.h>
#include "kggz.h"

int main(int argc, char **argv)
{
	KApplication *a;
	KGGZ *kggz;
	KAboutData *aboutData;

	aboutData = new KAboutData("kggz",
		I18N_NOOP("KGGZ"),
		"0.0.4",
		I18N_NOOP("Your most desired KDE network games launcher and chat system."),
		KAboutData::License_GPL,
		"(C) 2000 Josef Spillner",
		I18N_NOOP("Try it out!"),
		"http://mindx.sourceforge.net",
		"dr_maux@users.sourceforge.net");
	aboutData->addAuthor("Josef Spillner", I18N_NOOP("KDE2 port of the client"), "dr_maux@users.sourceforge.net", "http://mindx.sourceforge.net");
	aboutData->addCredit("Brian Cox", I18N_NOOP("(GGZ team)"), "bcox@users.sourceforge.net", "http://ggz.sourceforge.net");
	aboutData->addCredit("Rich Gade", I18N_NOOP("(GGZ team)"), "rgade@users.sourceforge.net", "http://ggz.sourceforge.net");
	aboutData->addCredit("Brent Hendricks", I18N_NOOP("(GGZ team)"), "bmh@users.sourceforge.net", "http://ggz.sourceforge.net");
	aboutData->addCredit("Justin Zaun", I18N_NOOP("(GGZ team)"), "jzaun@users.sourceforge.net", "http://ggz.sourceforge.net");
	aboutData->addCredit("Doug Hudson", I18N_NOOP("(GGZ team)"), "djh@users.sourceforge.net", "http://ggz.sourceforge.net");
	aboutData->addCredit("Ismael Orenstein", I18N_NOOP("(GGZ team)"), "perdig@users.sourceforge.net", "http://ggz.sourceforge.net");

	KCmdLineArgs::init(argc, argv, aboutData);

	a = new KApplication();
	kggz = new KGGZ();
	kggz->setCaption("GNU Gaming Zone");
	a->setMainWidget(kggz);
	return a->exec();
}
