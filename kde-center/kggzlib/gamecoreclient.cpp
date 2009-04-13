#include "gamecoreclient.h"

#include "embeddedcoreclient.h"
#include "playerlist.h"
#include "tablelist.h"
#include "roomlist.h"

//#include <kggzcore/coreclient.h>

#include <QLayout>
#include <QToolBar>
#include <QIcon>
#include <QAction>

#include <klocale.h>
#include <kiconloader.h>

#include <kchat.h>

GameCoreClient::GameCoreClient(KGGZCore::CoreClient *core, KGGZCoreLayer *layer)
: QMainWindow()
{
	m_ecc = new EmbeddedCoreClient(core, layer, false);

	// FIXME: the layout is nearly identical to the one in Vencedor, with the exception of the room list

	QWidget *centralwidget = new QWidget();
	setCentralWidget(centralwidget);

	QVBoxLayout *vbox = new QVBoxLayout();
	if(m_ecc->widget_rooms())
		vbox->addWidget(m_ecc->widget_rooms());
	vbox->addWidget(m_ecc->widget_players());
	vbox->addWidget(m_ecc->widget_tables());

	QHBoxLayout *hbox = new QHBoxLayout();
	hbox->addLayout(vbox);
	hbox->addWidget(m_ecc->widget_chat());

	centralwidget->setLayout(hbox);

	// FIXME: the toolbar is also partially identical (launch/join/spectate actions, maybe MOTD)
	// except for enabled launch button

	QToolBar *toolbar = new QToolBar();
	addToolBar(Qt::TopToolBarArea, toolbar);

	toolbar->addAction(m_ecc->action_launch());
	toolbar->addAction(m_ecc->action_join());
	toolbar->addAction(m_ecc->action_spectate());

	setWindowTitle(i18n("GGZ Control Panel"));
	resize(600, 700);
	show();
}

