//////////////////////////////////////////////////////////////////////
// KTicTacTux
// Copyright (C) 2001 Josef Spillner, dr_maux@users.sourceforge.net
// Published under GNU GPL conditions
//////////////////////////////////////////////////////////////////////

// KTicTacTux includes
#include "ktictactuxwin.h"

// KDE includes
#include <kapp.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <klocale.h>

// System includes
#include <stdio.h>

// Available command line options
static const KCmdLineOptions op[] = {
	{"o", I18N_NOOP("Launch game instead of joining"), 0},
	{"ggz", I18N_NOOP("Request GGZ game explicitely"), 0},
	{0, 0, 0}};

// Main function, including the about data
int main(int argc, char **argv)
{
	KApplication *a;
	KAboutData *aboutData;
	KTicTacTuxWin *ktictactuxwin;
	KCmdLineArgs *args;

	aboutData = new KAboutData("ktictactux",
		I18N_NOOP("KTicTacTux"),
		"0.0.5pre",
		I18N_NOOP("This is a TicTacToe game for KDE."),
		KAboutData::License_GPL,
		"(C) 2001 Josef Spillner",
		I18N_NOOP("This game can be played against the AI or over networks."),
		"http://mindx.sourceforge.net",
		"dr_maux@users.sourceforge.net");
	aboutData->addAuthor("Josef Spillner", I18N_NOOP("Inventor"), "dr_maux@users.sourceforge.net", "http://mindx.sourceforge.net");

	KCmdLineArgs::init(argc, argv, aboutData);
	KCmdLineArgs::addCmdLineOptions(op);
	args = KCmdLineArgs::parsedArgs();

	a = new KApplication();
	ktictactuxwin = new KTicTacTuxWin(NULL, "KTicTacTuxWin");
	if(args->isSet("ggz")) ktictactuxwin->tux()->setOpponent(PLAYER_NETWORK);
	ktictactuxwin->tux()->init();
	a->setMainWidget(ktictactuxwin);
	return a->exec();
}

