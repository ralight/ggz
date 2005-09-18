// GGZap - GGZ quick start application for the KDE panel
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

// GGZap includes
#include "ggzap.h"
#include "ggzap_tray.h"
#include "ggzap_handler.h"
#include "ggzap_gui.h"
#include "ggzap_guialt.h"

// KDE includes
#include <kapplication.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kconfig.h>

// Configuration includes
#include "config.h"

GGZap::GGZap(QWidget *parent, const char *name)
: QObject()
{
	GGZapTray *tray;
	KConfig *conf;
	QString gui;

	Q_UNUSED(parent);
	Q_UNUSED(name);

	conf = kapp->config();
	conf->setGroup("GUI");
	gui = conf->readEntry("Type");

	if(gui == "bluebox") m_gui = new GGZapGui();
	else m_gui = new GGZapGuiAlt();

	tray = new GGZapTray(m_gui, "GGZapTray");
	m_autolaunch = 0;

	m_handler = new GGZapHandler();

	connect(m_handler, SIGNAL(signalState(int)), SLOT(slotState(int)));
	connect(tray, SIGNAL(signalLaunch(QString, QString)), SLOT(slotLaunch(QString, QString)));
	connect(tray, SIGNAL(signalCancel()), SLOT(slotCancel()));
	connect(this, SIGNAL(signalMenu(int)), tray, SLOT(slotMenu(int)));

	m_gui->move(kapp->desktop()->width() / 2 - 125, kapp->desktop()->height() / 2 - 50);
}

GGZap::~GGZap()
{
	delete m_handler;
}

void GGZap::setModule(QString modulename)
{
	m_handler->setModule(modulename);
	m_autolaunch = 1;
	m_gui->setGame(modulename);
}

void GGZap::setFrontend(QString frontendtype)
{
	m_handler->setFrontend(frontendtype);
}

void GGZap::slotLaunch(QString name, QString frontend)
{
	setModule(name);
	setFrontend(frontend);
	launch();
}

void GGZap::slotCancel()
{
	m_handler->shutdown();
	m_gui->setProgress(0);
}

void GGZap::launch()
{
	int ret;

	if(!m_autolaunch)
	{
		m_gui->hide();
		//m_gui->showMinimized();
		return;
	}

	m_gui->show();
	m_gui->setProgress(1);
	ret = m_handler->init();
	switch(ret)
	{
		case GGZapHandler::error_module:
			KMessageBox::error(m_gui,
				i18n("No module found for this game."),
				i18n("Error!"));
			break;
		case GGZapHandler::error_username:
			KMessageBox::sorry(m_gui,
				i18n("You must configure a username first."),
				i18n("Start failed"));
			break;
		case GGZapHandler::error_none:
			// ok
			break;
	}
}

void GGZap::slotState(int state)
{
	switch(state)
	{
		case GGZapHandler::connected:
			m_gui->setProgress(2);
			break;
		case GGZapHandler::connectfail:
			slotCancel();
			KMessageBox::error(m_gui,
				i18n("Could not connect to server!"), i18n("Error!"));
			emit signalMenu(GGZapTray::menucancel);
			break;
		case GGZapHandler::negotiatefail:
			slotCancel();
			KMessageBox::error(m_gui,
				i18n("Wrong server version."), i18n("Error!"));
			emit signalMenu(GGZapTray::menucancel);
			break;
		case GGZapHandler::loggedin:
			m_gui->setProgress(3);
			break;
		case GGZapHandler::loginfail:
			slotCancel();
			KMessageBox::error(m_gui,
				i18n("Couldn't login!\nPlease use your registered user name."), i18n("Error!"));
			emit signalMenu(GGZapTray::menucancel);
			break;
		case GGZapHandler::joinedroom:
			m_gui->setProgress(4);
			break;
		case GGZapHandler::joinroomfail:
			slotCancel();
			KMessageBox::error(m_gui,
				i18n("Could not join the room!\nMaybe this game type isn't supported on the server?"), i18n("Error!"));
			emit signalMenu(GGZapTray::menucancel);
			break;
		case GGZapHandler::waiting:
			m_gui->setProgress(4);
			emit signalMenu(GGZapTray::menulaunch);
			break;
		case GGZapHandler::started:
			m_gui->setProgress(5);
			break;
		case GGZapHandler::startfail:
			slotCancel();
			KMessageBox::error(m_gui,
				i18n("Could not start the game!\nPlease make sure you have it installed."), i18n("Error!"));
			emit signalMenu(GGZapTray::menucancel);
			break;
		case GGZapHandler::error:
			KMessageBox::error(m_gui,
				i18n("An unknown error occured."), i18n("Error!"));
			emit signalMenu(GGZapTray::menucancel);
			break;
		case GGZapHandler::finish:
			//slotCancel();
			emit signalMenu(GGZapTray::menucancel);
			break;
	}
}

QWidget *GGZap::gui()
{
	return m_gui;
}

