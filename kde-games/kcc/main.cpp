///////////////////////////////////////////////////////////////////
// KCC
// Copyright (C) 2003, 2004 Josef Spillner, josef@ggzgamingzone.org
// Published under GNU GPL conditions
///////////////////////////////////////////////////////////////////

// KCC includes
#include "kccwin.h"

// KDE includes
#include <kapplication.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <klocale.h>

// Available command line options
static const KCmdLineOptions op[] = {
	{"ggz", I18N_NOOP("Request GGZ game explicitely"), 0},
	{"players <num>", I18N_NOOP("Number of local AI players"), 0},
	{0, 0, 0}};

// Main function, including the about data
int main(int argc, char **argv)
{
	KAboutData *aboutData;
	KCCWin *kccwin;
	KCmdLineArgs *args;

	aboutData = new KAboutData("kcc",
		I18N_NOOP("KCC"),
		"0.3",
		I18N_NOOP("This is a Chinese Checkers game for KDE."),
		KAboutData::License_GPL,
		"Copyright (C) 2003 - 2005 Josef Spillner",
		I18N_NOOP("This game is part of the GGZ Gaming Zone."),
		"http://www.ggzgamingzone.org/games/kcc/",
		"josef@ggzgamingzone.org");
	aboutData->addAuthor("Josef Spillner", I18N_NOOP("Inventor"), "josef@ggzgamingzone.org");
	aboutData->setTranslator(I18N_NOOP("TRANSLATOR-NAME"), I18N_NOOP("TRANSLATOR-EMAIL"));

	KCmdLineArgs::init(argc, argv, aboutData);
	KCmdLineArgs::addCmdLineOptions(op);
	args = KCmdLineArgs::parsedArgs();

	KApplication a;
	kccwin = new KCCWin(NULL);
	if(args->isSet("ggz"))
	{
		kccwin->enableNetwork(true);
		kccwin->kcc()->setOpponent(PLAYER_NETWORK);
	}
	else
	{
		kccwin->kcc()->setOpponent(PLAYER_AI);
		if(args->isSet("players"))
		{
			kccwin->kcc()->setPlayers(args->getOption("players").toInt());
		}
	}
	kccwin->kcc()->init();

	a.setMainWidget(kccwin);
	return a.exec();
}

