#include "shadowapp.h"

#include <kmenubar.h>
#include <kpopupmenu.h>

ShadowApp::ShadowApp()
: KMainWindow()
{
	KPopupMenu *menu_file, *menu_debug;

	client = NULL;
	clientggz = NULL;
	container = new ShadowContainer(this);
	net = new ShadowNet();

	menu_file = new KPopupMenu(this);
	menu_file->insertItem("Add game client", menuadd);
	menu_file->insertItem("Add GGZ game client", menuaddggz);
	menu_file->insertItem("Start", menustart);
	menu_file->insertSeparator();
	menu_file->insertItem("Quit", menuquit);

	menu_debug = new KPopupMenu(this);
	menu_debug->insertItem("Incoming message", menudebugin);
	menu_debug->insertItem("Outgoing message", menudebugout);

	menuBar()->insertItem("File", menu_file);
	menuBar()->insertItem("Debug", menu_debug);
	menuBar()->insertItem("Help", helpMenu());

	statusBar()->insertItem("Ready.", 1);

	connect(menu_file, SIGNAL(activated(int)), SLOT(slotMenu(int)));
	connect(menu_debug, SIGNAL(activated(int)), SLOT(slotMenu(int)));
	connect(net, SIGNAL(signalIncoming(const char *)), container, SLOT(slotIncoming(const char *)));
	connect(net, SIGNAL(signalOutgoing(const char *)), container, SLOT(slotOutgoing(const char *)));

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
				connect(client, SIGNAL(signalClient(const char*, const char*)), SLOT(slotClient(const char*, const char*)));
			}
			client->show();
			break;
		case menuaddggz:
			if(!clientggz)
			{
				clientggz = new ShadowClientGGZ(NULL);
				connect(clientggz, SIGNAL(signalClient(const char*, const char*)), SLOT(slotClient(const char*, const char*)));
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
			container->slotIncoming("In Test");
			break;
		case menudebugout:
			container->slotOutgoing("Out Test");
			break;
	}
}

void ShadowApp::slotClient(const char *name, const char *commandline)
{
	container->slotAdmin(QString("%1 (%2)").arg(name).arg(commandline));
	net->addClient(name, commandline);
}

