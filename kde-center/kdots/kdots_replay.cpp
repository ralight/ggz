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
#include <kstandarddirs.h>
#include <kmessagebox.h>

#include <qlabel.h>
#include <qlayout.h>
#include <qdir.h>
#include <qstringlist.h>
#include <qpushbutton.h>
#include <qlistwidget.h>

KDotsReplay::KDotsReplay()
: QWidget()
{
	QVBoxLayout *vbox, *vbox2;
	QHBoxLayout *hbox;
	QPushButton *ok;
	QStringList replays;
	QStringList::Iterator it;
	KStandardDirs d;

	dots = new QDots();
	dots->setFixedSize(400, 400);
	QPixmap pix(d.findResource("data", "kdots/dragon.png"));
	dots->setBackgroundImage(pix);

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
	connect(listbox, SIGNAL(itemActivated(QListWidgetItem*)), SLOT(slotLoad(QListWidgetItem*)));

	setWindowTitle(i18n("KDots Replay"));
	show();
}

KDotsReplay::~KDotsReplay()
{
}

void KDotsReplay::slotPosition(int value)
{
	label->setText(i18n("Position: %1", value));

	dots->step(value);
	dots->repaint();
}

void KDotsReplay::replay(QString filename)
{
	bool ret = dots->load(filename);
	if(!ret)
	{
		KMessageBox::error(this,
			i18n("The savegame could not be loaded"),
			i18n("Savegame error"));
		return;
	}

	dots->refreshBoard();

	slider->setRange(0, dots->positions());
	slider->setValue(0);
	slider->setEnabled(true);
	slotPosition(0);
}

void KDotsReplay::slotLoad(QListWidgetItem *item)
{
	QString filename = item->text();
	filename = QDir::home().path() + "/.ggz/games/kdots/" + filename;
	replay(filename);
}

