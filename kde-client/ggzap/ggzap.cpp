#include "ggzap.h"
#include "ggzap_tray.h"
#include "ggzap_handler.h"

#include <kapp.h>
#include <klocale.h>
#include <kmessagebox.h>

#include <qlayout.h>
#include <qframe.h>
#include <qpixmap.h>

#include <stdlib.h>
#include <stdio.h> /*temp*/

#include "config.h"

GGZap::GGZap(QWidget *parent, const char *name)
: QWidget(parent, name)
{
	QVBoxLayout *vbox;
	QHBoxLayout *hbox;
	QFrame *frame;
	GGZapTray *tray;

	tray = new GGZapTray(this, "GGZapTray");
	m_autolaunch = 0;

	frame = new QFrame(this);
	frame->setFixedSize(64, 64);
	frame->setBackgroundColor(QColor(255, 0, 0));
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
	connect(tray, SIGNAL(signalLaunch(char*, char*)), SLOT(slotLaunch(char*, char*)));
	connect(tray, SIGNAL(signalCancel()), SLOT(slotCancel()));
	connect(this, SIGNAL(signalMenu(int)), tray, SLOT(slotMenu(int)));

	startTimer(150);

	setFixedSize(250, 100);
	setCaption(i18n("GGZ Quick Launcher"));
	move(kapp->desktop()->width() / 2 - 125, kapp->desktop()->height() / 2 - 50);
}

GGZap::~GGZap()
{
	delete m_handler;
}

void GGZap::timerEvent(QTimerEvent *e)
{
	m_handler->process();
}

void GGZap::setModule(const char *modulename)
{
	m_handler->setModule(modulename);
	m_autolaunch = 1;
}

void GGZap::setFrontend(const char *frontendtype)
{
	m_handler->setFrontend(frontendtype);
}

void GGZap::fat(QLabel *label)
{
	//if(!label->text().contains("<b>"))
		//label->setText("<b>" + label->text() + "</b>");
	QFont font("helvetica", 11);
	font.setBold(TRUE);
	label->setFont(font, QFont::Black);
}

void GGZap::unfat(QLabel *label)
{
	//if(label->text().contains("<b>"))
		//label->setText(label->text().mid(3, label->text().length() - 7));
	QFont font("helvetica", 11);
	font.setBold(FALSE);
	label->setFont(font, QFont::Normal);
}

void GGZap::slotLaunch(char *name, char *frontend)
{
	setModule(name);
	setFrontend(frontend);
	launch();
}

void GGZap::slotCancel()
{
	m_handler->shutdown();
	unfat(m_start);
	unfat(m_wait);
	unfat(m_room);
	unfat(m_login);
	unfat(m_connect);
}

void GGZap::launch()
{
	if(!m_autolaunch)
	{
		showMinimized();
		return;
	}
	show();
	fat(m_connect);
	emit signalMenu(GGZapTray::menulaunch);
	m_handler->init();
printf("GGZap::launch() has emitted\n");
}

void GGZap::slotState(int state)
{
printf("GGZap::slotState(%i)\n", state);
	switch(state)
	{
		case GGZapHandler::connected:
			fat(m_login);
			break;
		case GGZapHandler::connectfail:
			KMessageBox::information(this, "Could not connect to server!", "Error!");
			slotCancel();
			emit signalMenu(GGZapTray::menucancel);
			break;
		case GGZapHandler::loggedin:
			fat(m_room);
			break;
		case GGZapHandler::loginfail:
			KMessageBox::information(this, "Couldn't login!\nPlease use your registered user name.", "Error!");
			slotCancel();
			emit signalMenu(GGZapTray::menucancel);
			break;
		case GGZapHandler::joinedroom:
			fat(m_wait);
			break;
		case GGZapHandler::joinroomfail:
			KMessageBox::information(this, "Could not join the room!\nMaybe this game type isn't supported on the server?", "Error!");
			slotCancel();
			emit signalMenu(GGZapTray::menucancel);
			break;
		case GGZapHandler::waiting:
			fat(m_wait);
			break;
		case GGZapHandler::started:
			fat(m_start);
			break;
		case GGZapHandler::startfail:
			KMessageBox::information(this, "Could not start the game!\nPlease make sure you have it installed.", "Error!");
			slotCancel();
			emit signalMenu(GGZapTray::menucancel);
			break;
		case GGZapHandler::finish:
			slotCancel();
			emit signalMenu(GGZapTray::menucancel);
			break;
	}
}

