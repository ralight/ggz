// Krosswater - Cross The Water for KDE
// Copyright (C) 2001, 2002 Josef Spillner, dr_maux@users.sourceforge.net
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

// Header file
#include "krosswater.h"

// KDE includes
#include <kmessagebox.h>
#include <kmenubar.h>
#include <kpopupmenu.h>
#include <klocale.h>

// Krosswater includes
#include "dlg_person.h"
#include "dlg_about.h"
#include "dlg_help.h"
#include "krosswater_move.h"

// Qt includes
#include <qlayout.h>
#include <qpainter.h>

// System includes
#include <iostream>
#include <unistd.h>

// GGZ includes
#include <ggz.h>

// Configuration includes
#include "config.h"

using namespace std;

// Constructor
Krosswater::Krosswater(QWidget *parent, const char *name)
: ZoneGGZModUI(parent, name)
{
	QVBoxLayout *vbox;
	DlgPerson *dlgperson;
	KPopupMenu *menu_game, *menu_help;
	QWidget *dummy;

	m_again = NULL;
	m_broken = 0;
	m_sleep = state_nosleep;
	m_turn = 0;

	dummy = new QWidget(this);
	setCentralWidget(dummy);

	qcw = new QCw(dummy, "qcw");

	m_statusframe = new QFrame(dummy);
	m_statusframe->setFixedHeight(25);
	m_statusframe->setErasePixmap(QPixmap(GGZDATADIR "/krosswater/gfx/bg.png"));
	showStatus(i18n("Uninitialized"));

	menu_game = new KPopupMenu(this);
	menu_game->insertItem(i18n("Quit"), this, SLOT(slotMenuQuit()));

	menu_help = new KPopupMenu(this);
	menu_help->insertItem(i18n("About"), this, SLOT(slotMenuAbout()));
	menu_help->insertItem(i18n("Help"), this, SLOT(slotMenuHelp()));

	menuBar()->insertItem(i18n("Game"), menu_game);
	menuBar()->insertSeparator();
	menuBar()->insertItem(i18n("Help"), menu_help);

	vbox = new QVBoxLayout(dummy, 0);
	vbox->add(qcw);
	vbox->add(m_statusframe);

	dlgperson = new DlgPerson(NULL, "DlgPerson");

	connect(dlgperson, SIGNAL(signalAccepted(int)), SLOT(slotSelected(int)));
	connect(qcw, SIGNAL(signalMove(int, int, int, int)), SLOT(slotMove(int, int, int, int)));

	ZoneRegister("Krosswater");
	connect(this, SIGNAL(signalZoneInput(int)), SLOT(slotZoneInput(int)));
	connect(this, SIGNAL(signalZoneReady()), SLOT(slotZoneReady()));
	connect(this, SIGNAL(signalZoneTurn()), SLOT(slotZoneTurn()));
	connect(this, SIGNAL(signalZoneTurnOver()), SLOT(slotZoneTurnOver()));
	connect(this, SIGNAL(signalZoneOver()), SLOT(slotZoneOver()));
	connect(this, SIGNAL(signalZoneInvalid()), SLOT(slotZoneInvalid()));

	startTimer(1000);

	setErasePixmap(QPixmap(GGZDATADIR "/krosswater/gfx/bg.png"));
	setCaption(i18n("Krosswater - Cross the Water!"));
	setFixedSize(700, 370);
}

// Destructor
Krosswater::~Krosswater()
{
}

// Handle protocol error
void Krosswater::protoError()
{
	m_broken = 1;
	KMessageBox::error(this,
		i18n("A protocol error has been detected. Aborting the game."),
		i18n("Client message"));
	close();
}

// Initialization after character selection
void Krosswater::slotSelected(int person)
{
	m_selectedperson = person;
	if(zonePlayers() == ZoneGamePlayers)
	{
		if((m_turn % ZoneGamePlayers) != zoneMe())
			showStatus(i18n("Game started"));
	}
	else showStatus(i18n("Waiting for other players..."));

	qcw->setPlayerPixmap(zoneMe(), person);

	for(int i = 0; i < ZoneGamePlayers; i++)
		if(i != zoneMe()) qcw->setPlayerPixmap(i, (person + 1) % 3);

	show();
}

// Send a move
void Krosswater::slotMove(int fromx, int fromy, int tox, int toy)
{
	if(!zoneTurn())
	{
		KMessageBox::sorry(this, i18n("Not your turn yet."), i18n("Client message"));
		return;
	}
	if(m_movelist.count())
	{
		KMessageBox::sorry(this, i18n("Moves are not finished yet."), i18n("Client message"));
		return;
	}

	showStatus(i18n("Sending move"));

	m_fromx = fromx;
	m_fromy = fromy;
	m_tox = tox;
	m_toy = toy;

	if((ggz_write_int(fd(), proto_move) < 0)
	|| (ggz_write_int(fd(), fromx) < 0)
	|| (ggz_write_int(fd(), fromy) < 0)
	|| (ggz_write_int(fd(), tox) < 0)
	|| (ggz_write_int(fd(), toy) < 0))
	{
		protoError();
	}
}

// Receive data from the server
void Krosswater::slotZoneInput(int op)
{
	int x, y, value;
	int maxplayers;
	int person;

	if(m_broken) return;

	showStatus(i18n("Receiving..."));

	if(op == proto_map_respond)
	{
		if((ggz_read_int(fd(), &x) < 0)
		|| (ggz_read_int(fd(), &y) < 0))
		{
			protoError();
			return;
		}
		qcw->setSize(x, y);
		for(int j = 0; j < y; j++)
			for(int i = 0; i < x; i++)
			{
				if(ggz_read_int(fd(), &value) < 0)
				{
					protoError();
					return;
				}
				qcw->setStone(i, j, value);
			}
		if(ggz_read_int(fd(), &maxplayers) < 0)
		{
			protoError();
			return;
		}
		qcw->resetPlayers();
		for(int i = 0; i < maxplayers; i++)
		{
			if((ggz_read_int(fd(), &x) < 0)
			|| (ggz_read_int(fd(), &y) < 0))
			{
				protoError();
				return;
			}
			if(i < ZoneGamePlayers)
			{
				qcw->addPlayer(x, y);

				if(i == zoneMe()) person = m_selectedperson;
				else person = (m_selectedperson + 1) % 3;
				qcw->setPlayerPixmap(i, person);
			}
		}
		qcw->enable();
		if((m_turn % ZoneGamePlayers) == zoneMe()) slotZoneTurn();
	}
	else if(op == proto_move_broadcast)
	{
		slotZoneBroadcast();
	}
	else if(op == proto_map_backtrace)
	{
		x = 0;
		if(ggz_read_int(fd(), &person) < 0)
		{
			protoError();
			return;
		}
		while(x != -1)
		{
			if(ggz_read_int(fd(), &x) < 0)
			{
				protoError();
				return;
			}
			if(x != -1)
			{
				if(ggz_read_int(fd(), &y) < 0)
				{
					protoError();
					return;
				}
				qcw->setStone(x, y, QCw::path);
			}
		}
		if(!m_again)
		{
			m_again = new DlgAgain(NULL, "DlgAgain");
			connect(m_again, SIGNAL(signalAgain()), SLOT(slotAgain()));
		}
		if(person == zoneMe()) m_again->setResult(i18n("You won the game."));
		else m_again->setResult(i18n("Sorry, but you lost the game."));
		m_again->show();
	}
	else protoError();
}

// Send ready status over the network
void Krosswater::slotZoneReady()
{
	showStatus(i18n("Send map"));

	if(ggz_write_int(fd(), proto_map) < 0)
		protoError();
}

// Inidicate that it's the player's turn
void Krosswater::slotZoneTurn()
{
	showStatus(i18n("Your turn"));
	qcw->setPlayerTurn(zoneMe());
}

// Display game over status
void Krosswater::slotZoneOver()
{
	showStatus(i18n("Game over"));

	qcw->disable();
	//KMessageBox::information(this, i18n("The game is over!"), i18n("Server message"));
}

// Indicate an invalid move
void Krosswater::slotZoneInvalid()
{
	KMessageBox::sorry(this, i18n("Invalid move!"), i18n("Server message"));
}

// Finish a turn
void Krosswater::slotZoneTurnOver()
{
	qcw->setStone(m_fromx, m_fromy, QCw::inactive);
	qcw->setStone(m_tox, m_toy, QCw::active);
}

// Quit the game
void Krosswater::slotMenuQuit()
{
	close();
}

// Display the about dialog
void Krosswater::slotMenuAbout()
{
	DlgAbout *dlgabout;

	dlgabout = new DlgAbout(NULL, "DlgAbout");
}

// Display the help dialog
void Krosswater::slotMenuHelp()
{
	DlgHelp *dlghelp;

	dlghelp = new DlgHelp(NULL, "DlgHelp");
}

// Show the game status
void Krosswater::showStatus(QString state)
{
	QPainter p;

	m_statusframe->erase();

	p.begin(m_statusframe);
	p.setFont(QFont("arial", 10));
	p.setPen(QPen(QColor(255, 255, 0)));
	p.drawText(0, 20, state);
	p.end();

	m_currentstate = state;
}

// Draw the map
void Krosswater::paintEvent(QPaintEvent *e)
{
	showStatus(m_currentstate);
}

// Receive a move
void Krosswater::slotZoneBroadcast()
{
	int fromx, tox, fromy, toy;

 	showStatus(i18n("Get move"));

	if((ggz_read_int(fd(), &fromx) < 0)
	|| (ggz_read_int(fd(), &fromy) < 0)
	|| (ggz_read_int(fd(), &tox) < 0)
	|| (ggz_read_int(fd(), &toy) < 0))
	{
		protoError();
		return;
	}

	m_movelist.append(new KrosswaterMove(fromx, fromy, tox, toy));
}

// Animation delay
void Krosswater::timerEvent(QTimerEvent *e)
{
	KrosswaterMove *m;

	showStatus(m_currentstate);

	if(m_movelist.count()) m = m_movelist.at(0);
	else m = NULL;

	switch(m_sleep)
	{
		case state_sleep1:
			qcw->setStoneState(m->fromx(), m->fromy(), QCw::fromframe);
			qcw->repaint();
			m_sleep = state_sleep2;
			break;
		case state_sleep2:
			qcw->setStoneState(m->tox(), m->toy(), QCw::toframe);
			qcw->repaint();
			m_sleep = state_sleep3;
			break;
		case state_sleep3:
			qcw->setStone(m->fromx(), m->fromy(), QCw::inactive);
			qcw->setStone(m->tox(), m->toy(), QCw::active);
			qcw->repaint();
			m_sleep = state_sleep4;
			break;
		case state_sleep4:
			qcw->setStoneState(m->tox(), m->toy(), QCw::inactive);
			qcw->setStoneState(m->fromx(), m->fromy(), QCw::inactive);
			qcw->setPlayerTurn(m_turn % ZoneGamePlayers);
			qcw->repaint();
			delete m;
			m_movelist.remove(m);
			m_sleep = state_nosleep;
			if((m_turn % ZoneGamePlayers) == zoneMe()) slotZoneTurn();
			break;
		case state_nosleep:
			if(m_movelist.count())
			{
				showStatus(i18n("Execute move"));
				if((m_turn % ZoneGamePlayers) == zoneMe()) m_turn++;
				qcw->setPlayerTurn(m_turn % ZoneGamePlayers);
				m_turn++;
				m_sleep = state_sleep1;
			}
			break;
		default:
			break;
	}
}

// Ask for another game
void Krosswater::slotAgain()
{
	m_again->close();
	if(ggz_write_int(fd(), proto_restart) < 0)
		protoError();
}

