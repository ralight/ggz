#include "kmodsniff.h"

#include <kapplication.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <klocale.h>

int main(int argc, char **argv)
{
	KModSniff *sniff;
	KAboutData *aboutData;

	aboutData = new KAboutData("ggzmodsniff",
		I18N_NOOP("ModSniffer"),
		"0.1",
		I18N_NOOP("Utily to find locally installed GGZ game modules."),
		KAboutData::License_GPL,
		"(C) 2000 - 2003 Josef Spillner\n"
		"The MindX Open Source Project",
		I18N_NOOP("GGZ ModSniffer"),
		"http://ggz.sourceforge.net/clients/modsniff",
		"josef@ggzgamingzone.org");

	aboutData->addAuthor("Josef Spillner", I18N_NOOP("Author"), "josef@ggzgamingzone.org", "http://mindx.dyndns.org");

	KCmdLineArgs::init(argc, argv, aboutData);

	KApplication a;
	sniff = new KModSniff();
	a.setMainWidget(sniff);
	return a.exec();
}

