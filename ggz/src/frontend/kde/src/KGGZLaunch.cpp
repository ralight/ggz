/////////////////////////////////////////////////////////////////////////////////////
//                                                                                 //
//    KGGZ - The KDE client for the GGZ Gaming Zone - Version 0.0.5pre             //
//    Copyright (C) 2000 - 2002 Josef Spillner - dr_maux@users.sourceforge.net     //
//    The MindX Open Source Project - http://mindx.sourceforge.net                 //
//    Published under GNU GPL conditions - view COPYING for details                //
//                                                                                 //
/////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////
//                                                                                 //
//    This program is free software; you can redistribute it and/or modify         //
//    it under the terms of the GNU General Public License as published by         //
//    the Free Software Foundation; either version 2 of the License, or            //
//    (at your option) any later version.                                          //
//                                                                                 //
//    This program is distributed in the hope that it will be useful,              //
//    but WITHOUT ANY WARRANTY; without even the implied warranty of               //
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                //
//    GNU General Public License for more details.                                 //
//                                                                                 //
//    You should have received a copy of the GNU General Public License            //
//    along with this program; if not, write to the Free Software                  //
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA    //
//                                                                                 //
/////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////
//                                                                                 //
// KGGZLaunch: Shows a dialog for game launching which allows changing parameters. //
//                                                                                 //
/////////////////////////////////////////////////////////////////////////////////////

// Header file
#include "KGGZLaunch.h"

// KGGZ includes
#include "KGGZCommon.h"

// KDE includes
#include <klocale.h>
//#define i18n(x) x

// Qt includes
#include <qlayout.h>
#include <qstring.h>
#include <stdlib.h>

KGGZLaunch::KGGZLaunch(QWidget *parent, const char *name)
: QWidget(parent, name)
{
	QVBoxLayout *vbox;
	QHBoxLayout *hbox;
	QLabel *label2, *label3;
	QPushButton *cancel;

	KGGZDEBUGF("KGGZLaunch::KGGZLaunch()\n");
	m_popup = NULL;
	m_array = NULL;
	m_assignment = NULL;
	m_playername = NULL;

	m_slider = new QSlider(this);
	m_slider->setOrientation(QSlider::Horizontal);
	m_slider->setMinValue(2);
	m_slider->setTickInterval(1);
	m_slider->setTickmarks(QSlider::Below);
	m_slider->setPageStep(1);

	m_listbox = new KListView(this);
	m_listbox->addColumn(i18n("Seat"));
	m_listbox->addColumn(i18n("Player"));

	m_edit = new QLineEdit(this);

	m_label = new QLabel("", this);
	label2 = new QLabel(i18n("Seat assignments:"), this);
	label3 = new QLabel(i18n("Game description:"), this);

	m_ok = new QPushButton("OK", this);
	m_ok->setEnabled(FALSE);
	cancel = new QPushButton("Cancel", this);

	vbox = new QVBoxLayout(this, 5);
	vbox->add(m_label);
	vbox->add(m_slider);
	vbox->add(label2);
	vbox->add(m_listbox);
	vbox->add(label3);
	vbox->add(m_edit);

	hbox = new QHBoxLayout(vbox, 5);
	hbox->add(m_ok);
	hbox->add(cancel);

	connect(m_listbox, SIGNAL(rightButtonPressed(QListViewItem*, const QPoint&, int)), SLOT(slotSelected(QListViewItem*, const QPoint&, int)));
	connect(m_slider, SIGNAL(valueChanged(int)), SLOT(slotChanged(int)));
	connect(m_ok, SIGNAL(clicked()), SLOT(slotAccepted()));
	connect(cancel, SIGNAL(clicked()), SLOT(close()));

	setFixedSize(350, 400);
	setCaption(i18n("Launch a game"));
	show();
}

KGGZLaunch::~KGGZLaunch()
{
	if(m_playername) free(m_playername);
}

void KGGZLaunch::slotSelected(QListViewItem *selected, const QPoint& point, int column)
{
	int seat;

	if(!selected) return;
	if(!selected->isSelectable()) return;
	if(selected == m_listbox->firstChild()) return;

	if(m_popup) delete m_popup;

	seat = selected->text(0).toInt();

	m_popup = new QPopupMenu(this);
	if(seat <= m_maxbots) m_popup->insertItem(typeName(seatbot), -seatbot);
	m_popup->insertItem(typeName(seatopen), -seatopen);
	m_popup->insertItem(typeName(seatreserved), -seatreserved);
#ifdef KGGZ_PATCH_SPECTATORS
	m_popup->insertItem(typeName(seatspectator), -seatspectator);
#endif
	m_popup->popup(point);

	connect(m_popup, SIGNAL(activated(int)), SLOT(slotActivated(int)));
}

void KGGZLaunch::slotActivated(int id)
{
	int seat;
	QListViewItem *tmp;

	tmp = m_listbox->selectedItem();
	if(!tmp) return;

	id = -id;

	seat = tmp->text(0).toInt();
	KGGZDEBUG("Got id: %i on seat: %i\n", id, seat);
	setSeatType(seat, id);
}

void KGGZLaunch::slotAccepted()
{
	emit signalLaunch();
}

void KGGZLaunch::slotChanged(int value)
{
	QString str, str2;

	if((m_assignment) && (m_assignment->at(value - 1) == 0))
	{
		m_slider->setValue(m_curplayers);
		return;
	}

	m_curplayers = value;
	str.setNum(value);
	str2.setNum(m_slider->maxValue());
	m_label->setText(i18n("Number of player: ") + str + i18n(" (out of ") + str2 + ")");

	if(m_assignment)
	{
		for(int i = value; i < m_slider->maxValue(); i++)
			setSeatType(i, seatunused);
		for(int i = 0; i < value; i++)
			if(seatType(i) == seatunused) setSeatType(i, seatopen);
	}
}

const char *KGGZLaunch::description()
{
	return m_edit->text().latin1();
}

int KGGZLaunch::seats()
{
	return m_slider->value();
}

void KGGZLaunch::initLauncher(char *playername, int maxplayers, int maxbots, int maxspectators)
{
	QString str;

	KGGZDEBUGF("KGGZLaunch::initLauncher(%s, %i, %i, %i)\n", playername, maxplayers, maxbots, maxspectators);
	if(m_array)
	{
		KGGZDEBUG("Critical: array initialized twice!\n");
		return;
	}

	m_slider->setMaxValue(maxplayers);
	m_slider->setValue(maxplayers);
	m_curplayers = maxplayers;
	m_playername = strdup(playername);
	KGGZDEBUG("array: create with %i elements...\n", maxplayers);
	m_array = new QByteArray(maxplayers);
	m_assignment = new QByteArray(maxplayers);
	KGGZDEBUG("array: done: size = %i\n", m_array->size());

	for(int i = 0; i < maxplayers; i++)
	{
		str.setNum(i);
		(void)new QListViewItem(m_listbox, str);
	}

	if(maxplayers >= 2)
	{
		setSeatType(0, seatplayer);
		for(int i = 1; i < maxplayers; i++)
		{
			if(i <= maxbots) setSeatType(i, seatbot);
			else setSeatType(i, seatopen);
		}
	}

	m_ok->setEnabled(TRUE);

	m_maxbots = maxbots;

	KGGZDEBUGF("KGGZLaunch::initLauncher() done\n");
}

int KGGZLaunch::seatType(int seat)
{
	int ret;

	if((!m_array) || ((int)m_array->size() <= seat)) return seatunknown;
	ret = m_array->at(seat);
	return ret;
}

void KGGZLaunch::setSeatAssignment(int seat, int enabled)
{
	if(!m_assignment)
	{
		KGGZDEBUG("Critical! No combination matrix found!\n");
		return;
	}

	m_assignment->at(seat) = enabled;
}

void KGGZLaunch::setSeatType(int seat, int seattype)
{
	QListViewItem *tmp;

	if(!m_array)
	{
		KGGZDEBUG("Critical! No array present.\n");
		return;
	}
	if(seat >= (int)m_array->size())
	{
		KGGZDEBUG("Critical: not so many seats here (%i/%i)!\n", seat, seattype);
		return;
	}

	tmp = m_listbox->firstChild();
	if(!tmp)
	{
		KGGZDEBUG("error!\n");
		return;
	}

	for(int i = 0; i < seat; i++)
	{
		tmp = tmp->itemBelow();
		if(!tmp)
		{
			KGGZDEBUG("error!\n");
			return;
		}
	}

	tmp->setText(1, typeName(seattype));

	KGGZDEBUG("within setSeatType: arry size is: %i (set at %i)\n", m_array->size(), seat);
	m_array->at(seat) = seattype;
	KGGZDEBUG("after setSeatType: arry size is: %i\n", m_array->size());
}

QString KGGZLaunch::typeName(int seattype)
{
	QString ret;

	switch(seattype)
	{
		case seatplayer:
			ret.append(m_playername);
			break;
		case seatopen:
			ret.append(i18n("Open"));
			break;
		case seatreserved:
			ret.append(i18n("Reserved"));
			break;
		case seatbot:
			ret.append(i18n("Bot"));
			break;
		case seatunused:
			ret.append(i18n("(unused)"));
			break;
		case seatspectator:
			ret.append(i18n("Spectator"));
			break;
		default:
			ret.append(i18n("unknown"));
	}

	KGGZDEBUG("return: %s for %i\n", ret.latin1(), seattype);
	return ret;
}

