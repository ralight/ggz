/*
 * The GGZ Gaming Zone Metaserver Project
 * Copyright (C) 2001 Josef Spillner, dr_maux@users.sourceforge.net
 * Published under GNU GPL conditions.
 */
	
#include <qapplication.h>
#include "qmetagui.h"

int main(int argc, char **argv)
{
	QApplication a(argc, argv);
	QMetaGUI *metagui;

	metagui = new QMetaGUI();
	a.setMainWidget(metagui);
	metagui->show();
	return a.exec();
}

