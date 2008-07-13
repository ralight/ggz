// ShadowBridge - Game developer tool to visualize network protocols
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
#include "shadowapp.h"

// ShadowBridge includes
#include "shadowcontainer.h"
#include "shadowclient.h"
#include "shadowclientggz.h"
#include "shadownet.h"

// KDE includes
#include <kmenubar.h>
#include <kpopupmenu.h>
#include <kmessagebox.h>
#include <klocale.h>
#include <kstatusbar.h>

ShadowApp::ShadowApp()
: KMainWindow()
{
	KPopupMenu *menu_file, *menu_debug;

	client = NULL;
	clientggz = NULL;
	container = new ShadowContainer(this);
	net = new ShadowNet();

	menu_file = new KPopupMenu(this);
	menu_file->insertItem(i18n("Add game client"), menuadd);
	menu_file->insertItem(i18n("Add GGZ game client"), menuaddggz);
	menu_file->insertItem(i18n("Start"), menustart);
	menu_file->insertSeparator();
	menu_file->insertItem(i18n("Quit"), menuquit);

	menu_debug = new KPopupMenu(this);
	menu_debug->insertItem(i18n("Incoming message"), menudebugin);
	menu_debug->insertItem(i18n("Outgoing message"), menudebugout);
	menu_debug->insertItem(i18n("Force synchronizing"), menudebugsync);

	menuBar()->insertItem(i18n("File"), menu_file);
	menuBar()->insertItem(i18n("Debug"), menu_debug);
	menuBar()->insertItem(i18n("Help"), helpMenu());

	statusBar()->insertItem(i18n("Ready."), 1);

	connect(menu_file, SIGNAL(activated(int)), SLOT(slotMenu(int)));
	connect(menu_debug, SIGNAL(activated(int)), SLOT(slotMenu(int)));
	connect(net, SIGNAL(signalIncoming(const QString&)), container, SLOT(slotIncoming(const QString&)));
	connect(net, SIGNAL(signalOutgoing(const QString&)), container, SLOT(slotOutgoing(const QString&)));
	connect(net, SIGNAL(signalError(const QString&)), SLOT(slotError(const QString&)));
	connect(container, SIGNAL(signalActivated(int)), net, SLOT(slotActivated(int)));

	setCentralWidget(container);
	setCaption("Shadow Bridge");
	show();
}

ShadowApp::~ShadowApp()
{
	delete net;
}

void ShadowApp::slotMenu(int id)
{
	switch(id)
	{
		case menuadd:
			if(!client)
			{
				client = new ShadowClient(NULL);
				connect(client, SIGNAL(signalClient(const QString&, const QString&, const QString&)),
					SLOT(slotClient(const QString&, const QString&, const QString&)));
			}
			client->show();
			break;
		case menuaddggz:
			if(!clientggz)
			{
				clientggz = new ShadowClientGGZ(NULL);
				connect(clientggz, SIGNAL(signalClient(const QString&, const QString&, const QString&)),
					SLOT(slotClient(const QString&, const QString&, const QString&)));
			}
			clientggz->show();
			break;
		case menustart:
			net->start();
			break;
		case menuquit:
			close();
			break;
		case menudebugin:
			container->slotIncoming(i18n("In Test"));
			break;
		case menudebugout:
			container->slotOutgoing(i18n("Out Test"));
			break;
		case menudebugsync:
			net->sync();
			break;
	}
}

void ShadowApp::slotClient(const QString& exec, const QString& fdin, const QString& fdout)
{
	container->slotAdmin(QString("%1 [%2:%3]").arg(exec).arg(fdin).arg(fdout));
	net->addClient(exec, fdin, fdout);
}

void ShadowApp::slotError(const QString& error)
{
	KMessageBox::error(this, error, i18n("Net Error!"));
}

