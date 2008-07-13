//////////////////////////////////////////////////////////////////////
// KConnectX
// Copyright (C) 2006 Josef Spillner <josef@ggzgamingzone.org>
// Published under GNU GPL conditions
//////////////////////////////////////////////////////////////////////

// Header file
#include "kconnectx.h"

// KConnectX includes
#include "opt.h"

// GGZ-KDE-Games includes
#include <qsvgpixmap.h>

// KDE includes
#include <klocale.h>
#include <kmessagebox.h>
#include <kapplication.h>
#include <kdebug.h>
#include <kstandarddirs.h>

// Qt includes
#include <qlayout.h>
#include <qpixmap.h>
#include <qpushbutton.h>

// System includes
#include <stdio.h>
#include <stdlib.h>

// Constructor
KConnectX::KConnectX(QWidget *parent, const char *name)
: QWidget(parent, name)
{
	m_widgetbox = NULL;

	m_proto = new Proto();

	connect(m_proto,
		SIGNAL(signalOptionsRequest(char, char, char, char, char, char)),
		SLOT(slotOptionsRequest(char, char, char, char, char, char)));
	connect(m_proto,
		SIGNAL(signalOptions(char, char, char)),
		SLOT(slotOptions(char, char, char)));
	connect(m_proto,
		SIGNAL(signalMoveRequest()),
		SLOT(slotMoveRequest()));
	connect(m_proto,
		SIGNAL(signalMoveResponse(char, int)),
		SLOT(slotMoveResponse(char, int)));
	connect(m_proto,
		SIGNAL(signalMove(int)),
		SLOT(slotMove(int)));
	connect(m_proto,
		SIGNAL(signalOver(char)),
		SLOT(slotOver(char)));

	setMinimumSize(200, 200);
}

// Destructor
KConnectX::~KConnectX()
{
	delete m_proto;
}

void KConnectX::slotOptionsRequest(char minboardwidth, char maxboardwidth,
	char minboardheight, char maxboardheight,
	char minconnectlength, char maxconnectlength)
{
	emit signalStatus(i18n("Request for options"));

	Opt opt(this);
	opt.setBoardWidthRange(minboardwidth, maxboardwidth);
	opt.setBoardHeightRange(minboardheight, maxboardheight);
	opt.setConnectLengthRange(minconnectlength, maxconnectlength);
	opt.exec();

	char boardwidth = opt.boardWidth();
	char boardheight = opt.boardHeight();
	char connectlength = opt.connectLength();

	emit signalStatus(i18n("Send options..."));
	m_proto->sendOptions(boardwidth, boardheight, connectlength);

	// inform ourselves since gameserver only informs other player
	slotOptions(boardwidth, boardheight, connectlength);
}

void KConnectX::slotOptions(char boardwidth, char boardheight, char connectlength)
{
	emit signalStatus(i18n("Reading options"));

	m_boardwidth = boardwidth;
	m_boardheight = boardheight;
	Q_UNUSED(connectlength);

	m_buttons.clear();
	m_widgets.clear();
	m_stacks.clear();
	m_colours.clear();
	// FIXME: setAutoDelete() or similar?

	m_widgetbox = new QGridLayout(this, boardheight + 1, boardwidth);

	for(int i = 0; i < boardwidth; i++)
	{
		QPushButton *bu = new QPushButton(i18n("v"), this);
		bu->show();
		m_buttons[bu] = i;
		m_widgetbox->addWidget(bu, 0, i);
		connect(bu, SIGNAL(clicked()), SLOT(slotInput()));

		for(int j = 0; j < boardheight; j++)
		{
			QFrame *fr = new QFrame(this);
			fr->show();
			fr->setFrameStyle(QFrame::Panel | QFrame::Sunken);
			m_widgets[j * boardwidth + i] = fr;
			m_widgetbox->addWidget(fr, j + 1, i);
		}
	}
}

void KConnectX::slotMoveRequest()
{
	emit signalStatus(i18n("Your turn"));
}

void KConnectX::slotMoveResponse(char movestatus, int move)
{
	emit signalStatus(i18n("Move result"));
	switch(movestatus)
	{
		case Proto::allok:
			doMove("yellow", move);
			break;
		case Proto::errstate:
			KMessageBox::error(this,
				i18n("Wrong state!"),
				i18n("Invalid move"));
			break;
		case Proto::errturn:
			KMessageBox::error(this,
				i18n("Not your turn!"),
				i18n("Invalid move"));
			break;
		case Proto::errbound:
			KMessageBox::error(this,
				i18n("Move out of bounds!"),
				i18n("Invalid move"));
			break;
		case Proto::errfull:
			KMessageBox::error(this,
				i18n("Column is full!"),
				i18n("Invalid move"));
			break;
	}
}

void KConnectX::slotMove(int move)
{
	emit signalStatus(i18n("Opponent move"));

	doMove("red", move);
}

void KConnectX::doMove(QString colour, int column)
{
	int j = (m_boardheight - 1) - m_stacks[column];

	int acc = j * m_boardwidth + column;
	if(!m_widgets.contains(acc))
	{
		// FIXME: this is a bogus move from the server!
		kdDebug() << "SERVER SENT BOGUS MOVE " << column << endl;
		return;
	}

	m_colours[acc] = colour;
	m_stacks[column] += 1;

	drawBoard();
}

void KConnectX::slotOver(char winner)
{
	emit signalStatus(i18n("Game over"));

	// FIXME: allow playing another game
	emit signalGameOver();
}

// Evaluate your turn (after click)
void KConnectX::slotInput()
{
	const QPushButton *button = dynamic_cast<const QPushButton*>(sender());
	int column = m_buttons[button];

	kdDebug() << "that's column " << column << endl;

	emit signalStatus(i18n("Send move..."));

	m_proto->sendMove(column);
}

// Ask for yet another game (some people can't get enough)
void KConnectX::announce(QString str)
{
	int ret;

	ret = KMessageBox::questionYesNo(this,
		str + "\n\n" + i18n("Play another game?"),
		i18n("Game over"));

	switch(ret)
	{
		case KMessageBox::Yes:
			break;
		case KMessageBox::No:
			kapp->quit();
			break;
	}
}

// Statistics
void KConnectX::statistics()
{
//	proto->getStatistics();

//	emit signalNetworkScore(proto->stats[0], proto->stats[1], proto->stats[2]);
}

Proto *KConnectX::proto() const
{
	return m_proto;
}

void KConnectX::drawBoard()
{
	for(int i = 0; i < m_boardwidth; i++)
	{
		for(int j = 0; j < m_boardheight; j++)
		{
			int key = j * m_boardwidth + i;
			if(!m_colours.contains(key)) continue;
			QWidget *w = m_widgets[key];
			QString colour = m_colours[key];
			QString filename = KGlobal::dirs()->findResource("data", "kconnectx/" + colour + ".svg");
			w->setPaletteBackgroundPixmap(QSvgPixmap::pixmap(filename, w->width(), w->height()));
		}
	}
}

void KConnectX::resizeEvent(QResizeEvent *e)
{
	Q_UNUSED(e);

	drawBoard();
}

