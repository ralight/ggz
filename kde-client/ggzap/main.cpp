#include "ggzap.h"

#include <kapp.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <klocale.h>

int main(int argc, char **argv)
{
	KApplication *a;
	GGZap *zap;
	KAboutData *aboutData;
	KCmdLineArgs *args;
	static const KCmdLineOptions op[] = {
		{"module <modulename>", I18N_NOOP("Specify module to launch"), 0},
		{"frontend <frontendtype>", I18N_NOOP("Preferred frontend"), 0},
		{0, 0, 0}
	};

	aboutData = new KAboutData("ggzap",
		I18N_NOOP("GGZ Quick Launcher"),
		"0.1",
		I18N_NOOP("Tool to start GGZ games directly."),
		KAboutData::License_GPL,
		"(C) 2000,2001 Josef Spillner\n"
		"The MindX Open Source Project",
		I18N_NOOP("GGZap"),
		"http://mindx.sourceforge.net/games/kggz",
		"dr_maux@users.sourceforge.net");

	aboutData->addAuthor("Josef Spillner", I18N_NOOP("Author"), "dr_maux@users.sourceforge.net", "http://mindx.sourceforge.net");

	KCmdLineArgs::init(argc, argv, aboutData);
	KCmdLineArgs::addCmdLineOptions(op);

	args = KCmdLineArgs::parsedArgs();

	a = new KApplication();
	zap = new GGZap();
	if(args->isSet("module")) zap->setModule(args->getOption("module"));
	if(args->isSet("frontend")) zap->setFrontend(args->getOption("frontend"));
	zap->launch();
	a->setMainWidget(zap);
	return a->exec();
}

