///////////////////////////////////////////////////////////////
//
// KDots
// Connect the Dots game for KDE, using the Dots classes
// Copyright (C) 2001 - 2004 Josef Spillner
// josef@ggzgamingzone.org
// http://www.ggzgamingzone.org/games/kdots/
//
///////////////////////////////////////////////////////////////

#include "kdots_replay.h"
#include "qdots.h"

#include <klocale.h>

#include <qlayout.h>
#include <qdir.h>
#include <qstringlist.h>
#include <qpushbutton.h>

KDotsReplay::KDotsReplay(QWidget *parent, const char *name)
{
	QVBoxLayout *vbox, *vbox2;
	QHBoxLayout *hbox;
	QPushButton *ok;
	QStringList replays;
	QStringList::Iterator it;

	dots = new QDots(this);
	dots->setFixedSize(400, 400);

	slider = new QSlider(QSlider::Horizontal, this);
	slider->setEnabled(false);

	ok = new QPushButton(i18n("Close"), this);

	listbox = new QListBox(this);	
	QDir dir(QDir::home().path() + "/.ggz/games/kdots");
	replays = dir.entryList("*");
	for(it = replays.begin(); it != replays.end(); it++)
	{
		if((*it) == ".") continue;
		if((*it) == "..") continue;
		listbox->insertItem((*it));
	}

	label = new QLabel(i18n("Position"), this);

	hbox = new QHBoxLayout(this, 5);
	vbox2 = new QVBoxLayout(hbox, 5);
	vbox2->add(listbox);
	vbox2->add(ok);
	vbox = new QVBoxLayout(hbox, 5);
	vbox->add(dots);
	vbox->add(label);
	vbox->add(slider);

	connect(ok, SIGNAL(clicked()), SLOT(close()));
	connect(slider, SIGNAL(valueChanged(int)), SLOT(slotPosition(int)));
	connect(listbox, SIGNAL(highlighted(const QString&)), SLOT(slotLoad(const QString&)));

	setCaption(i18n("KDots Replay"));
	show();
}

KDotsReplay::~KDotsReplay()
{
}

void KDotsReplay::slotPosition(int value)
{
	label->setText(i18n("Position: %1").arg(value));

	dots->step(value);
	dots->repaint();
}

void KDotsReplay::slotLoad(const QString& filename)
{
	dots->load(QDir::home().path() + "/.ggz/games/kdots/" + filename);
	dots->refreshBoard();

	slider->setRange(0, dots->positions());
	slider->setValue(0);
	slider->setEnabled(true);
	slotPosition(0);
}

