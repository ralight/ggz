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

#include "kdots_about.h"

#include <qpushbutton.h>
#include <qlayout.h>
#include <qlabel.h>

KDotsAbout::KDotsAbout(QWidget *parent, char *name)
: QWidget(parent, name)
{
	QVBoxLayout *vbox;
	QLabel *label;
	QPushButton *ok;

	label = new QLabel("<b>KDots</b><br>"
		"A dots game for KDE<br><br>"
		"Ideas (but actually no code) stolen from Rich Gade's Gtk+ version :-)<br><br>"
		"Copyright (C) 2001 Josef Spillner<br>"
		"The MindX Open Source Project",
		this);

	ok = new QPushButton("OK", this);

	vbox = new QVBoxLayout(this, 5);
	vbox->add(label);
	vbox->add(ok);

	connect(ok, SIGNAL(clicked()), SLOT(slotAccepted()));

	setCaption("About KDots");
	setFixedSize(300, 200);
	show();
}

KDotsAbout::~KDotsAbout()
{
}

void KDotsAbout::slotAccepted()
{
	close();
}
