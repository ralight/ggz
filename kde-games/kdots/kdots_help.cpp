///////////////////////////////////////////////////////////////
//
// KDots
// Connect the Dots game for KDE, using the Dots classes
// Copyright (C) 2001 Josef Spillner
// dr_maux@users.sourceforge.net
// The MindX Open Source Project
// http://mindx.sourceforge.net/games/kdots/
//
///////////////////////////////////////////////////////////////

#include "kdots_help.h"

KDotsHelp::KDotsHelp(QWidget *parent, char *name)
: QWidget(parent, name)
{

	setCaption("KDots Help");
	resize(300, 200);
	show();
}

KDotsHelp::~KDotsHelp()
{
}

void KDotsHelp::slotAccepted()
{
}
