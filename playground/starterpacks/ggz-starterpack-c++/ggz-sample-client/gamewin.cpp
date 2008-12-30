//
// GGZ Starterpack for C++ - Sample Client
// Copyright (C) 2008 GGZ Development Team
//
// This code is made available as public domain; you can use it as a base
// for your own game, as long as its licence is compatible with the libraries
// you use.

#include "gamewin.h"

// KDE includes
#include <kmenu.h>
#include <kmenubar.h>
#include <klocale.h>
#include <kstatusbar.h>
#include <kconfig.h>
#include <kconfiggroup.h>
#include <kapplication.h>
#include <kmessagebox.h>
#include <kstandarddirs.h>
#include <kdebug.h>
#include <kiconloader.h>

#include <kggzlib/connectiondialog.h>
#include <kggzlib/kggzcorelayer.h>
#include <kggzgames/kggzseatsdialog.h>

#include <kggzcore/player.h>

#include <qdir.h>
#include <qstringlist.h>
#include <kglobal.h>

GameWin::GameWin()
: KMainWindow()
{
	QWidget *cwidget = new QWidget();
	cwidget->setFixedSize(QSize(500, 500));
	setCentralWidget(cwidget);

	m_proto = NULL;
	m_mod = NULL;
	m_networked = false;

	mgame = new KMenu(this);
	mgame->setTitle(i18n("Game"));
	action_connect = mgame->addAction(KIconLoader::global()->loadIcon("network-connect", KIconLoader::Small), i18n("Connect"));
	action_ggzplayers = mgame->addAction(KIconLoader::global()->loadIcon("ggz", KIconLoader::Small), i18n("Seats && Spectators"));
	mgame->addSeparator();
	action_quit = mgame->addAction(KIconLoader::global()->loadIcon("application-exit", KIconLoader::Small), i18n("Quit"));

	menuBar()->addMenu(mgame);
	menuBar()->addMenu(helpMenu());

	statusBar()->insertItem(i18n("Status"), 1, 1);
	statusBar()->insertItem(i18n("No HELLO message in sight"), 2, 1);

	connect(mgame, SIGNAL(triggered(QAction*)), SLOT(slotMenu(QAction*)));

	setCaption(i18n("GGZ Starterpack for C++ - Sample Client"));
	show();
}

void GameWin::slotStatus(const QString &status)
{
	statusBar()->changeItem(status, 1);
}

void GameWin::slotMenu(QAction *action)
{
	if(action == action_connect)
	{
		connectcore();
	}
	else if(action == action_ggzplayers)
	{
		KGGZSeatsDialog *dlg = new KGGZSeatsDialog();
		dlg->show();
	}
	else if(action == action_quit)
	{
		close();
	}
}

void GameWin::enableNetwork(bool enabled)
{
	m_networked = enabled;

	action_connect->setEnabled(!enabled);
	action_ggzplayers->setEnabled(enabled);

	if(enabled)
	{
		m_proto = new ggz_starterpack();
		m_mod = new KGGZMod::Module("ggz-sample-client-c++");

		connect(m_proto, SIGNAL(signalNotification(ggz_starterpackOpcodes::Opcode, const msg&)), SLOT(slotPacket(ggz_starterpackOpcodes::Opcode, const msg&)));
		connect(m_proto, SIGNAL(signalError()), SLOT(slotError()));
		connect(m_mod, SIGNAL(signalError()), SLOT(slotError()));
		connect(m_mod, SIGNAL(signalNetwork(int)), SLOT(slotNetwork(int)));
		//connect(m_mod, SIGNAL(signalEvent(const KGGZMod::Event&)), SLOT(slotEvent(const KGGZMod::Event&)));
	}
}

void GameWin::slotNetwork(int fd)
{
	kDebug() << "Network activity...";

	m_proto->ggzcomm_set_fd(fd);
	m_proto->ggzcomm_network_main();
}

void GameWin::slotPacket(ggz_starterpackOpcodes::Opcode opcode, const msg& message)
{
	kDebug() << "Network data arriving from packet reader";

	Q_UNUSED(message);

	switch(opcode)
	{
		case ggz_starterpackOpcodes::message_hello:
			slotStatus(i18n("Received HELLO message!"));
			break;
		default:
			slotError();
			break;
	}
}

void GameWin::slotError()
{
	enableNetwork(false);
}

void GameWin::connectcore()
{
	ConnectionDialog dialog(this);
	dialog.setGame("Starterpack", "1.0");
	if(dialog.exec() == QDialog::Accepted)
	{
		enableNetwork(true);
	}
}
