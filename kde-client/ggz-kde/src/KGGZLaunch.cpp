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
#include "KGGZInput.h"

// KDE includes
#include <klocale.h>
#include <kmessagebox.h>

// Qt includes
#include <qlayout.h>
#include <qstring.h>
#include <qslider.h>
#include <qpopupmenu.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qimage.h>

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
	m_input = NULL;
	m_namedbots = NULL;
	m_buddies = NULL;
	m_seat = -1;

	m_slider = new QSlider(this);
	m_slider->setOrientation(QSlider::Horizontal);
	m_slider->setMinValue(1);
	m_slider->setTickInterval(1);
	m_slider->setTickmarks(QSlider::Below);
	m_slider->setPageStep(1);

	m_listbox = new KListView(this);
	m_listbox->addColumn(i18n("Seat"));
	m_listbox->addColumn(i18n("Player"));
	m_listbox->addColumn(i18n("Reservation"));
	m_listbox->setSorting(-1, TRUE);

	m_edit = new QLineEdit(this);

	m_label = new QLabel("", this);
	label2 = new QLabel(i18n("Seat assignments (right-click to change):"), this);
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

	connect(m_listbox,
		SIGNAL(rightButtonPressed(QListViewItem*, const QPoint&, int)),
		SLOT(slotSelected(QListViewItem*, const QPoint&, int)));
	connect(m_slider, SIGNAL(valueChanged(int)), SLOT(slotChanged(int)));
	connect(m_ok, SIGNAL(clicked()), SLOT(slotAccepted()));
	connect(cancel, SIGNAL(clicked()), SLOT(close()));

	setFixedSize(350, 400);
	setCaption(i18n("Launch a game"));
	show();
}

KGGZLaunch::~KGGZLaunch()
{
}

void KGGZLaunch::slotSelected(QListViewItem *selected, const QPoint& point, int column)
{
	int seat;

	Q_UNUSED(column);

	if(!selected) return;
	if(!selected->isSelectable()) return;
	if(selected == m_listbox->firstChild()) return;

	if(m_popup) delete m_popup;

	seat = selected->text(0).toInt();

	m_popup = new QPopupMenu(this);
	m_popup->insertItem(i18n("Leave seat open"), -seatopen);
	if((m_curbots < m_maxbots) || ((m_curbots = m_maxbots) && (m_maxbots > 0)))
		m_popup->insertItem(i18n("Add some bot"), -seatbot);
	m_popup->insertSeparator();
	m_popup->insertItem(i18n("Reserve..."), -seatprereserved);
	if(m_namedbots)
	{
		m_popup->insertItem(i18n("Individual bots"), m_namedbots);
	}
	if(m_buddies)
	{
		m_popup->insertItem(i18n("Buddies"), m_buddies);
	}
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

	seat = tmp->text(0).toInt() - 1;
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
	m_label->setText(i18n("Number of players: ") + str + i18n(" (out of ") + str2 + ")");

	if(m_assignment)
	{
		for(int i = value; i < m_slider->maxValue(); i++)
			setSeatType(i, seatunused);
		for(int i = 0; i < value; i++)
			if(seatType(i) == seatunused) setSeatType(i, seatopen);
	}
}

QString KGGZLaunch::description()
{
	return m_edit->text();
}

int KGGZLaunch::seats()
{
	return m_slider->value();
}

void KGGZLaunch::initLauncher(QString playername, int maxplayers, int maxbots)
{
	QString str;

	KGGZDEBUGF("KGGZLaunch::initLauncher(%s, %i, %i)\n", playername.latin1(), maxplayers, maxbots);
	if(m_array)
	{
		KGGZDEBUG("Critical: array initialized twice!\n");
		return;
	}

	m_slider->setMaxValue(maxplayers);
	m_slider->setValue(maxplayers);
	m_curplayers = maxplayers;
	m_playername = playername;
	KGGZDEBUG("array: create with %i elements...\n", maxplayers);
	m_array = new QByteArray(maxplayers);
	m_assignment = new QByteArray(maxplayers);
	KGGZDEBUG("array: done: size = %i\n", m_array->size());

	for(int i = maxplayers - 1; i >= 0; i--)
	{
		str.setNum(i + 1);
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
	m_curbots = maxbots;

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
	int oldtype;
	QString pixmap;

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

	if(seattype == seatprereserved)
	{
		if(!m_input)
		{
			m_input = new KGGZInput(NULL, NULL, i18n("Reservation"),
				i18n("Name of the player whom the seat is reserved for"));
			connect(m_input, SIGNAL(signalText(QString)),
				SLOT(slotReservation(QString)));
		}
		m_seat = seat;
		m_listbox->setEnabled(false);
		m_input->show();
		return;
	}

	oldtype = seatType(seat);

	tmp->setText(1, typeName(seattype));

	if(seattype == seatopen) pixmap = "guest.png";
	else if(seattype == seatbot) pixmap = "bot.png";
	else if(seattype == seatreserved) pixmap = "player.png";
	if(seattype <= seatbotlist)
	{
		pixmap = "bot.png";
		if(m_reservations.contains(-seattype))
		{
			if(m_grubbies.contains(m_reservations[-seattype]))
			{
				pixmap = "botgrubby.png";
				tmp->setText(1, i18n("Chat bot"));
			}
		}
	}
	QPixmap pix = QPixmap(KGGZ_DIRECTORY "/images/icons/players/" + pixmap);
	tmp->setPixmap(1, pix);

	if(seattype == seatplayer)
	{
		QPixmap pix1 = QPixmap(KGGZ_DIRECTORY "/images/icons/players/player.png");
		QPixmap pix2 = QPixmap(KGGZ_DIRECTORY "/images/icons/players/you.png");
		QPixmap pix3 = composite(pix1, pix2);
		tmp->setPixmap(1, pix3);
	}

	if(seattype <= seatbuddylist)
	{
		QPixmap pix1 = QPixmap(KGGZ_DIRECTORY "/images/icons/players/player.png");
		QPixmap pix2 = QPixmap(KGGZ_DIRECTORY "/images/icons/players/buddy.png");
		QPixmap pix3 = composite(pix1, pix2);
		tmp->setPixmap(1, pix3);
	}

	if(seattype != seatreserved)
	{
		freeReservation(tmp->text(2));
		if(m_reservations.contains(-seattype))
		{
			tmp->setText(2, m_reservations[-seattype]);
			addReservation(-seattype);
		}
		else if(seattype == seatplayer)
		{
			tmp->setText(2, m_playername);
		}
		else
		{
			tmp->setText(2, QString::null);
		}
	}

	if(seattype <= seatbuddylist) seattype = seatreserved;
	else if(seattype <= seatbotlist)
	{
		int orig = seattype;
		seattype = seatbot;
		if(m_reservations.contains(-orig))
		{
			if(m_grubbies.contains(m_reservations[-orig]))
			{
				seattype = seatreserved;
			}
		}
	}

	if((seattype == seatbot) && (oldtype != seatbot)) m_curbots++;
	else if((oldtype == seatbot) && (seattype != seatbot)) m_curbots--;

	m_array->at(seat) = seattype;
}

void KGGZLaunch::addReservation(int id)
{
	if(m_buddies) m_buddies->setItemEnabled(id, false);
	if(m_namedbots) m_namedbots->setItemEnabled(id, false);
}

void KGGZLaunch::freeReservation(QString name)
{
	if(name.isNull()) return;

	QMap<int, QString>::Iterator it;
	for(it = m_reservations.begin(); it != m_reservations.end(); it++)
	{
		if(it.data() == name)
		{
			int id = it.key();
			if(m_buddies) m_buddies->setItemEnabled(id, true);
			if(m_namedbots) m_namedbots->setItemEnabled(id, true);
			return;
		}
	}
}

QString KGGZLaunch::typeName(int seattype)
{
	QString ret;

	if(seattype <= seatbuddylist)
	{
		return i18n("Reserved");
	}
	if(seattype <= seatbotlist)
	{
		return i18n("Named Bot");
	}

	switch(seattype)
	{
		case seatplayer:
			ret.append(i18n("(Yourself)"));
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
		default:
			ret.append(i18n("unknown"));
	}

	KGGZDEBUG("return: %s for %i\n", ret.latin1(), seattype);
	return ret;
}

void KGGZLaunch::slotReservation(QString player)
{
	QListViewItem *tmp;

	for(int i = 0; i < seats(); i++)
	{
		if(player == reservation(i))
		{
			KMessageBox::error(this,
				i18n("The player %1 is already on a seat.").arg(player),
				i18n("Double assignment"));
			m_listbox->setEnabled(true);
			return;
		}
	}

	tmp = m_listbox->selectedItem();
	if(tmp)
	{
		freeReservation(tmp->text(2));

		int id = seatreserved;

		QMap<int, QString>::Iterator it;
		for(it = m_reservations.begin(); it != m_reservations.end(); it++)
		{
			if(it.data() == player)
			{
				id = -(it.key());
				if(m_buddies) m_buddies->setItemEnabled(id, false);
				if(m_namedbots) m_namedbots->setItemEnabled(id, false);
			}
		}

		tmp->setText(2, player);
		setSeatType(m_seat, id);
	}
	m_listbox->setEnabled(true);
}

QString KGGZLaunch::reservation(int seat)
{
	QListViewItem *tmp;

	if((seat < 0) || (seat >= (int)m_array->size())) return QString::null;

	tmp = m_listbox->firstChild();
	if(!tmp) return QString::null;

	for(int i = 0; i < seat; i++)
		if(tmp) tmp = tmp->itemBelow();

	if(!tmp) return QString::null;
	return tmp->text(2);
}

void KGGZLaunch::addBot(QString botname, QString botclass)
{
	if(!m_namedbots)
	{
		m_namedbots = new QPopupMenu();
		connect(m_namedbots, SIGNAL(activated(int)), SLOT(slotActivated(int)));
	}
	int id = -seatbotlist + m_namedbots->count();
	if(botclass == "grubby")
	{
		if(m_grubbies.count() == 0)
		{
			m_namedbots->insertSeparator(0);
		}
		m_grubbies[botname] = 1;
		QPixmap pix = QPixmap(KGGZ_DIRECTORY "/images/icons/players/botgrubby.png");
		m_namedbots->insertItem(pix, QString("%1 (Grubby)").arg(botname), id, 0);
	}
	else
	{
		QPixmap pix = QPixmap(KGGZ_DIRECTORY "/images/icons/players/bot.png");
		m_namedbots->insertItem(pix, QString("%1 (%1)").arg(botname).arg(botclass), id);
	}
	m_reservations[id] = botname;
}

void KGGZLaunch::addBuddy(QString buddyname)
{
	if(!m_buddies)
	{
		m_buddies = new QPopupMenu();
		connect(m_buddies, SIGNAL(activated(int)), SLOT(slotActivated(int)));
	}
	int id = -seatbuddylist + m_buddies->count();
	QPixmap pix = QPixmap(KGGZ_DIRECTORY "/images/icons/players/player.png");
	QPixmap pix2 = QPixmap(KGGZ_DIRECTORY "/images/icons/players/buddy.png");
	QPixmap pix3 = composite(pix, pix2);
	m_buddies->insertItem(pix3, QString("%1").arg(buddyname), id);
	m_reservations[id] = buddyname;
}

// FIXME: taken from KGGZUsers
QPixmap KGGZLaunch::composite(QPixmap bottom, QPixmap top)
{
	QPixmap comp;

	QImage topim = top.convertToImage();
	QImage bottomim = bottom.convertToImage();

	for(int j = 0; j < bottom.height(); j++)
		for(int i = 0; i < bottom.width(); i++)
		{
			if(qAlpha(topim.pixel(i, j)))
				bottomim.setPixel(i, j, topim.pixel(i, j));
		}
	comp.convertFromImage(bottomim);
	return comp;
}

