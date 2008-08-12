#include "vencedor.h"

#include <QTreeWidget>
#include <QLayout>

#include <kggzcore/room.h>
#include <kggzcore/misc.h>

Vencedor::Vencedor(QString url)
: QMainWindow()
{
	m_core = new KGGZCore::CoreClient(this);

	connect(m_core,
		SIGNAL(signalFeedback(KGGZCore::CoreClient::FeedbackMessage, KGGZCore::Error::ErrorCode)),
		SLOT(slotFeedback(KGGZCore::CoreClient::FeedbackMessage, KGGZCore::Error::ErrorCode)));
	connect(m_core,
		SIGNAL(signalAnswer(KGGZCore::CoreClient::AnswerMessage)),
		SLOT(slotAnswer(KGGZCore::CoreClient::AnswerMessage)));
	connect(m_core,
		SIGNAL(signalEvent(KGGZCore::CoreClient::EventMessage)),
		SLOT(slotEvent(KGGZCore::CoreClient::EventMessage)));

	m_core->setUrl(url);
	m_core->initiateLogin();

	QWidget *centralwidget = new QWidget();
	setCentralWidget(centralwidget);

	m_roomwidget = new QTreeWidget();
	m_roomwidget->setColumnCount(1);
	m_roomwidget->setHeaderLabels(QStringList("Rooms"));

	QVBoxLayout *layout = new QVBoxLayout();
	layout->addWidget(m_roomwidget);
	//layout->addStretch();
	centralwidget->setLayout(layout);

	setWindowTitle("Vencedor");
	show();
}

void Vencedor::slotFeedback(KGGZCore::CoreClient::FeedbackMessage message, KGGZCore::Error::ErrorCode error)
{
	KGGZCore::Room *room;

	Q_UNUSED(error);

	switch(message)
	{
		case KGGZCore::CoreClient::connection:
			break;
		case KGGZCore::CoreClient::negotiation:
			break;
		case KGGZCore::CoreClient::login:
			break;
		case KGGZCore::CoreClient::roomenter:
			//if(error == KGGZCore::Error::no_status)
			room = m_core->room();
			break;
		case KGGZCore::CoreClient::chat:
			break;
		case KGGZCore::CoreClient::channel:
			break;
		case KGGZCore::CoreClient::logout:
			break;
	}
}

void Vencedor::slotAnswer(KGGZCore::CoreClient::AnswerMessage message)
{
	QList<QTreeWidgetItem*> items;

	switch(message)
	{
		case KGGZCore::CoreClient::roomlist:
			m_core->initiateRoomChange(m_core->roomnames().at(0));
			for(int i = 0; i < m_core->roomnames().count(); i++)
			{
				items.append(new QTreeWidgetItem((QTreeWidget*)0,
					QStringList(m_core->roomnames().at(i))));
			}
			m_roomwidget->insertTopLevelItems(0, items);
			break;
		case KGGZCore::CoreClient::typelist:
			break;
		case KGGZCore::CoreClient::motd:
			qDebug("MOTD web url: %s", qPrintable(m_core->textmotd()));
			qDebug("MOTD web text:\n%s", qPrintable(m_core->webmotd()));
			break;
	}
}

void Vencedor::slotEvent(KGGZCore::CoreClient::EventMessage message)
{
	switch(message)
	{
		case KGGZCore::CoreClient::state_changed:
			qDebug("* state: %s", qPrintable(KGGZCore::Misc::statename(m_core->state())));
			break;
		case KGGZCore::CoreClient::players_changed:
			break;
		case KGGZCore::CoreClient::rooms_changed:
			break;
		case KGGZCore::CoreClient::neterror:
			break;
		case KGGZCore::CoreClient::protoerror:
			break;
		case KGGZCore::CoreClient::libraryerror:
			break;
	}
}
