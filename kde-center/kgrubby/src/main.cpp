#include <kapplication.h>
#include <klocale.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>

#include "toplevel.h"

int main(int argc, char **argv)
{
	Toplevel *top;
	KAboutData *about;

	about = new KAboutData("kgrubby",
		0,
		ki18n("KGrubby"),
		"0.1",
		ki18n("Configuration of Grubby, the GGZ chatbot"),
		KAboutData::License_GPL,
		ki18n("Copyright (C) 2005 - 2008 Josef Spillner"),
		KLocalizedString(),
		"http://www.ggzgamingzone.org/utils/kgrubby/",
		"josef@ggzgamingzone.org");
	about->addAuthor(ki18n("Josef Spillner"), ki18n("Developer"), "josef@ggzgamingzone.org");

	KCmdLineArgs::init(argc, argv, about);

	KApplication a;
	top = new Toplevel();
	//a.setMainWidget(top);
	return a.exec();
}

