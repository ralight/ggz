#include "ggzap.h"
#include "ggzap_handler.h"

#include <klocale.h>
#include <kmessagebox.h>

#include <qlayout.h>
#include <qframe.h>
#include <qpixmap.h>

#include <stdlib.h>

#include "config.h"

GGZap::GGZap(QWidget *parent, char *name)
: QWidget(parent, name)
{
	QVBoxLayout *vbox;
	QHBoxLayout *hbox;
	QFrame *frame;

	frame = new QFrame(this);
	frame->setFixedSize(64, 64);
	frame->setBackgroundColor(QColor(255.0, 0.0, 0.0));
	frame->setBackgroundPixmap(QPixmap(KGGZ_DIRECTORY "/ggzap/ggzap.png"));

	m_connect = new QLabel(i18n("Connect to server"), this);
	m_login = new QLabel(i18n("Login"), this);
	m_room = new QLabel(i18n("Enter room"), this);
	m_wait = new QLabel(i18n("Waiting for other players"), this);
	m_start = new QLabel(i18n("Start game"), this);

	hbox = new QHBoxLayout(this, 10);
	hbox->add(frame);
	vbox = new QVBoxLayout(hbox, 10);
	vbox->add(m_connect);
	vbox->add(m_login);
	vbox->add(m_room);
	vbox->add(m_wait);
	vbox->add(m_start);

	m_handler = new GGZapHandler();

	connect(m_handler, SIGNAL(signalState(int)), SLOT(slotState(int)));

	startTimer(500);

	resize(250, 100);
	setCaption(i18n("GGZ Quick Launcher"));
	show();
}

GGZap::~GGZap()
{
}

void GGZap::timerEvent(QTimerEvent *e)
{
	m_handler->process();
}

void GGZap::setModule(const char *modulename)
{
	fat(m_connect);
	m_handler->init(modulename);
}

void GGZap::setFrontend(const char *frontendtype)
{
	m_handler->setFrontend(frontendtype);
}

void GGZap::fat(QLabel *label)
{
	label->setText("<b>" + label->text() + "</b>");
}

void GGZap::slotState(int state)
{
	switch(state)
	{
		case GGZapHandler::connected:
			fat(m_login);
			break;
		case GGZapHandler::connectfail:
			KMessageBox::information(this, "Could not connect to server!", "Error!");
			exit(-1);
			break;
		case GGZapHandler::loggedin:
			fat(m_room);
			break;
		case GGZapHandler::loginfail:
			KMessageBox::information(this, "Couldn't login!\nPlease use your registered user name.", "Error!");
			exit(-1);
			break;
		case GGZapHandler::joinedroom:
			fat(m_wait);
			break;
		case GGZapHandler::joinroomfail:
			KMessageBox::information(this, "Could not join the room!\nMaybe this game type isn't supported on the server?", "Error!");
			exit(-1);
			break;
		case GGZapHandler::waiting:
			fat(m_wait);
			break;
		case GGZapHandler::started:
			fat(m_start);
			break;
		case GGZapHandler::startfail:
			KMessageBox::information(this, "Could not start the game!\nPlease make sure you have it installed.", "Error!");
			exit(-1);
			break;
	}
}

