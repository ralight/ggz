#include "krosswater.h"

#include <kmessagebox.h>
#include <kmenubar.h>
#include <kpopupmenu.h>

#include "dlg_person.h"
#include "dlg_about.h"
#include "dlg_help.h"
#include "dlg_again.h"

#include <qlayout.h>
#include <qpainter.h>

#include <iostream>
#include <unistd.h>

#include <easysock.h>

#include "config.h"

Krosswater::Krosswater(QWidget *parent, char *name)
: ZoneGGZModUI(parent, name)
{
	QVBoxLayout *vbox; //, *vbox2;
	DlgPerson *dlgperson;
	KMenuBar *menu;
	KPopupMenu *menu_game, *menu_help;
	QWidget *dummy;

	qcw = new QCw(this, "qcw");

	dummy = new QWidget(this);
	dummy->setFixedHeight(20);

	m_statusframe = new QFrame(this);
	//m_statusframe->setFrameStyle(QFrame::Panel | QFrame::Sunken);
	m_statusframe->setFixedHeight(25);
	m_statusframe->setBackgroundPixmap(QPixmap(GGZDATADIR "/krosswater/gfx/bg.png"));
	//m_status = new QLabel("", statusframe);
	//m_status->setBackgroundPixmap(QPixmap(GGZDATADIR "/krosswater/gfx/bg.png"));
	//m_status->setFixedSize(300, 20);
	showStatus("Uninitialized");
	//vbox2 = new QVBoxLayout(statusframe, 2);
	//vbox2->add(m_status);

	menu_game = new KPopupMenu(this);
	menu_game->insertItem("Quit", this, SLOT(slotMenuQuit()));

	menu_help = new KPopupMenu(this);
	menu_help->insertItem("About", this, SLOT(slotMenuAbout()));
	menu_help->insertItem("Help", this, SLOT(slotMenuHelp()));

	menu = new KMenuBar(this);
	menu->insertItem("Game", menu_game);
	menu->insertSeparator();
	menu->insertItem("Help", menu_help);

	vbox = new QVBoxLayout(this, 5);
	vbox->add(dummy);
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

	setBackgroundPixmap(QPixmap(GGZDATADIR "/krosswater/gfx/bg.png"));
	setCaption("Krosswater - Cross the Water!");
	setFixedSize(700, 370);

	cout << "Krosswater: ready!" << endl;
}

Krosswater::~Krosswater()
{
}

void Krosswater::slotSelected(int person)
{
	m_selectedperson = person;
	cout << "zonePlayers: " << zonePlayers() << endl;
	cout << "ZoneMaxplayers: " << ZoneMaxplayers << endl;
	cout << "Person: " << person << endl;
	if(zonePlayers() == ZoneMaxplayers) showStatus("Game started");
	else showStatus("Waiting for other players...");

	for(int i = 0; i < zonePlayers(); i++)
	{
		if(i == zoneMe()) person = m_selectedperson;
		else person = (m_selectedperson + 1) % 3;
		if(qcw) qcw->setPlayerPixmap(i, person);
		cout << "qcw->setPlayer " << person << " - " << m_selectedperson<< endl;
	}

	show();
}

void Krosswater::slotMove(int fromx, int fromy, int tox, int toy)
{
	cout << "Move from " << fromx << ", " << fromy << endl;
	cout << "to " << tox << ", " << toy << endl;

	if(!zoneTurn())
	{
		KMessageBox::information(this, "Not your turn yet.", "Client message");
		return;
	}

	showStatus("Sending move");

	m_fromx = fromx;
	m_fromy = fromy;
	m_tox = tox;
	m_toy = toy;

	if((es_write_int(fd(), proto_move) < 0)
	|| (es_write_int(fd(), fromx) < 0)
	|| (es_write_int(fd(), fromy) < 0)
	|| (es_write_int(fd(), tox) < 0)
	|| (es_write_int(fd(), toy) < 0))
	{
		printf("error in protocol (3)\n");
	}
}

void Krosswater::slotZoneInput(int op)
{
	int x, y, value;
	DlgAgain *again;
	int maxplayers;
	int person;

	printf("CHILD: slotZoneInput() -> Type: %i\n", op);

	if(op == proto_map_respond)
	{
		cout << "Map response!" << endl;
		if((es_read_int(fd(), &x) < 0)
		|| (es_read_int(fd(), &y) < 0))
		{
			printf("error in protocol (6)\n");
			return;
		}
		cout << "Size: " << x << ", " << y << endl;
		qcw->setSize(x, y);
		for(int j = 0; j < y; j++)
			for(int i = 0; i < x; i++)
			{
				if(es_read_int(fd(), &value) < 0)
				{
					printf("error in protocol (7)\n");
					return;
				}
				qcw->setStone(i, j, value);
			}
		if(es_read_int(fd(), &maxplayers) < 0)
		{
			printf("error in protocol (6n)\n");
			return;
		}
		cout << "players: " << maxplayers << endl;
		for(int i = 0; i < maxplayers; i++)
		{
			if((es_read_int(fd(), &x) < 0)
			|| (es_read_int(fd(), &y) < 0))
			{
				printf("error in protocol (6m)\n");
				return;
			}
			cout << "Player found: " << x << ", " << y << endl;
			if(qcw) qcw->addPlayer(x, y);
		}
	}

	if(op == proto_move_broadcast)
	{
		slotZoneBroadcast();
	}

	if(op == proto_map_backtrace)
	{
		x = 0;
		while(x != -1)
		{
			if(es_read_int(fd(), &x) < 0)
			{
				printf("error in protocol (9)\n");
				return;
			}
			if(x != -1)
			{
				if(es_read_int(fd(), &y) < 0)
				{
					printf("error in protocol (9n)\n");
					return;
				}
				cout << "backtrace: << " << x << " << " << y << endl;
				qcw->setStone(x, y, 3);
			}
		}
		again = new DlgAgain(NULL, "DlgAgain");
	}
}

void Krosswater::slotZoneReady()
{
	printf("CHILD: slotZoneReady()\n");
	/*if(es_write_int(fd(), proto_helloworld) < 0)
	{
		printf("error in protocol 1\n");
	}
	if(es_write_string(fd(), "Hello World.") < 0)
	{
		printf("error in protocol 2\n");
	}*/
	if(es_write_int(fd(), proto_map) < 0)
	{
		printf("error in protocol 1b\n");
	}
	printf("CHILD: sent thingies...\n");
}

void Krosswater::slotZoneTurn()
{
	cout << "SLOT: Turn!" << endl;
}

void Krosswater::slotZoneOver()
{
	KMessageBox::information(this, "The game is over!", "Server message");
	// fooo
}

void Krosswater::slotZoneInvalid()
{
	KMessageBox::information(this, "Invalid move!", "Server message");
}

void Krosswater::slotZoneTurnOver()
{
	cout << "Apply move from " << m_fromx << ", " << m_fromy << endl;
	cout << "to " << m_tox << ", " << m_toy << endl;
	qcw->setStone(m_fromx, m_fromy, 0);
	qcw->setStone(m_tox, m_toy, 1);
}


void Krosswater::slotMenuQuit()
{
	close();
}

void Krosswater::slotMenuAbout()
{
	DlgAbout *dlgabout;

	dlgabout = new DlgAbout(NULL, "DlgAbout");
}

void Krosswater::slotMenuHelp()
{
	DlgHelp *dlghelp;

	dlghelp = new DlgHelp(NULL, "DlgHelp");
}

void Krosswater::showStatus(char *state)
{
	QPainter p;

	//m_status->clear();

	m_statusframe->erase();

	p.begin(m_statusframe);
	p.setFont(QFont("arial", 10));
	p.setPen(QPen(QColor(255.0, 255.0, 0.0)));
	p.drawText(0, 20, state);
	p.end();

	m_currentstate = state;
	printf("State: %s\n", state);
}

void Krosswater::paintEvent(QPaintEvent *e)
{
	showStatus(m_currentstate);
}

void Krosswater::slotZoneBroadcast()
{
	int fromx, tox, fromy, toy;

 	showStatus("Get move");

	if((es_read_int(fd(), &fromx) < 0)
	|| (es_read_int(fd(), &fromy) < 0)
	|| (es_read_int(fd(), &tox) < 0)
	|| (es_read_int(fd(), &toy) < 0))
	{
		cout << "Error in protocol (5b)" << endl;
		return;
	}

	cout << "Got move: " << fromx << ", " << fromy << " -> " << tox << ", " << toy << endl;
	qcw->setStoneState(fromx, fromy, -1);
	qcw->repaint();
	sleep(1);
	qcw->setStoneState(tox, toy, -2);
	qcw->repaint();
	sleep(1);
	qcw->setStone(fromx, fromy, 0);
	qcw->repaint();
	//sleep(1);
	qcw->setStone(tox, toy, 1);
	qcw->setStoneState(tox, toy, 0);
	qcw->setStoneState(fromx, fromy, 0);
	qcw->repaint();
}
