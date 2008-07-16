/////////////////////////////////////////////////////////////////////
//
// KDots: Connect the Dots game for KDE 4
// http://www.ggzgamingzone.org/gameclients/kdots/
//
// Copyright (C) 2001 - 2008 Josef Spillner <josef@ggzgamingzone.org>
// Published under the conditions of the GNU GPL, see COPYING
//
/////////////////////////////////////////////////////////////////////

#include "kdots_replay.h"
#include "qdots.h"

#include <klocale.h>

#include <QLabel>
#include <QLayout>
#include <QDir>
#include <QStringList>
#include <QPushButton>
#include <QListWidget>

KDotsReplay::KDotsReplay()
: QWidget()
{
	QVBoxLayout *vbox, *vbox2;
	QHBoxLayout *hbox;
	QPushButton *ok;
	QStringList replays;
	QStringList::Iterator it;

	dots = new QDots();
	dots->setFixedSize(400, 400);

	slider = new QSlider(Qt::Horizontal);
	slider->setEnabled(false);

	ok = new QPushButton(i18n("Close"));

	listbox = new QListWidget();	
	QDir dir(QDir::home().path() + "/.ggz/games/kdots");
	replays = dir.entryList(QStringList("*"));
	for(it = replays.begin(); it != replays.end(); it++)
	{
		if((*it) == ".") continue;
		if((*it) == "..") continue;
		QListWidgetItem *item = new QListWidgetItem((*it));
		listbox->addItem(item);
	}

	label = new QLabel(i18n("Position"));

	hbox = new QHBoxLayout();
	setLayout(hbox);
	vbox2 = new QVBoxLayout();
	hbox->addLayout(vbox2);
	vbox2->addWidget(listbox);
	vbox2->addWidget(ok);
	vbox = new QVBoxLayout();
	hbox->addLayout(vbox);
	vbox->addWidget(dots);
	vbox->addWidget(label);
	vbox->addWidget(slider);

	connect(ok, SIGNAL(clicked()), SLOT(close()));
	connect(slider, SIGNAL(valueChanged(int)), SLOT(slotPosition(int)));
	connect(listbox, SIGNAL(highlighted(const QString&)), SLOT(slotLoad(const QString&)));

	setWindowTitle(i18n("KDots Replay"));
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

