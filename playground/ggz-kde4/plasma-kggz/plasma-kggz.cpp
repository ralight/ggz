#include "plasma-kggz.h"

#include "configwidget.h"

#include <qpainter.h>
#include <qfontmetrics.h>
#include <QtGui/QGraphicsSceneMouseEvent>

#include <kconfigdialog.h>
#include <kprocess.h>
#include <kshell.h>
 
#include <plasma/svg.h>
#include <plasma/theme.h>

#include <kggzcore/room.h>
#include <kggzcore/misc.h>
#include <kggzcore/table.h>
#include <kggzcore/player.h>
 
PlasmaKGGZ::PlasmaKGGZ(QObject *parent, const QVariantList &args)
	: Plasma::Applet(parent, args),
	m_svg(this),
	m_icon("ggz"),
	m_config(0),
	m_core(0)
{
	setBackgroundHints(DefaultBackground);
	setHasConfigurationInterface(true);

	activity(i18n("GGZ Gaming Zone"));

	m_svg.setImagePath("widgets/plot-background");

	resize(300, 600);
}
 
PlasmaKGGZ::~PlasmaKGGZ()
{
	if(hasFailedToLaunch())
	{
	}
	else
	{
	}

	delete m_config;
}
 
void PlasmaKGGZ::init()
{
	if (m_icon.isNull())
	{
		setFailedToLaunch(true, "GGZ icon not found.");
		return;
	}

	KConfigGroup cg = config();
	if(cg.readEntry("username").isEmpty())
	{
		activity(i18n("Unconfigured."));
		return;
	}

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

	activity(i18n("Connecting..."));

	m_core->setUrl("ggz://localhost:5688");
	m_core->setUsername(cg.readEntry("username"));
	m_core->initiateLogin();
} 
 
void PlasmaKGGZ::paintInterface(QPainter *p,
	const QStyleOptionGraphicsItem *option, const QRect &contentsRect)
{
	p->setRenderHint(QPainter::SmoothPixmapTransform);
	p->setRenderHint(QPainter::Antialiasing);

	m_svg.resize((int)contentsRect.width(), (int)contentsRect.height());
	m_svg.paint(p, (int)contentsRect.left(), (int)contentsRect.top());

	p->drawPixmap(7, 0, m_icon.pixmap((int)contentsRect.width(), (int)contentsRect.width() - 14));
	p->save();
	p->setPen(Qt::white);
	p->drawText(contentsRect, Qt::AlignBottom | Qt::AlignHCenter, m_activity);
	p->restore();
}

void PlasmaKGGZ::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	kDebug() << "mousePressEvent!";

	if(event->button() == Qt::LeftButton)
	{
		if(event->pos().x() == 0)
		{
			// ...
		}

		QString url = KShell::quoteArg(m_core->url());

		KProcess::startDetached("ggz-gtk", QStringList() << url);
	}

	event->ignore();
}

void PlasmaKGGZ::createConfigurationInterface(KConfigDialog *parent)
{
	m_config = new ConfigWidget(parent);

	parent->addPage(m_config, parent->windowTitle(), icon());

	KConfigGroup cg = config();
	m_config->setUsername(cg.readEntry("username"));
	m_config->setPassword(cg.readEntry("password"));

	connect(parent, SIGNAL(applyClicked()), SLOT(slotConfiguration()));
	connect(parent, SIGNAL(okClicked()), SLOT(slotConfiguration()));
}

void PlasmaKGGZ::activity(QString activity)
{
	m_activity = activity;
	update();
}

void PlasmaKGGZ::slotConfiguration()
{
	kDebug() << "slotConfiguration!";

	KConfigGroup cg = config();
	cg.writeEntry("username", m_config->username());
	cg.writeEntry("password", m_config->password());
	cg.sync();

	if((!m_core) || (m_config->username() != m_core->username()))
	{
		delete m_core;
		init();
	}
}

void PlasmaKGGZ::checkTables()
{
	QList<KGGZCore::Table*> tables = m_core->room()->tables();
	qDebug() << "#count:" << tables.size();
	for(int i = 0; i < tables.size(); i++)
	{
		KGGZCore::Table *table = tables.at(i);
		qDebug() << "# -" << table->description() << endl;
		QList<KGGZCore::Player*> players = table->players();
		for(int j = 0; j < players.size(); j++)
		{
			KGGZCore::Player *p = players.at(j);
			qDebug() << "#  -" << p->name() << p->type();
			qDebug() << "   -" << m_core->username() << KGGZCore::Player::reserved;
			if(p->type() == KGGZCore::Player::reserved)
			{
				if(m_core->username() == p->name())
				{
					qDebug() << "GO!";
					activity(i18n("You're invited to table %1 (%2)!", i, table->description()));
				}
			}
		}
	}
}

void PlasmaKGGZ::slotFeedback(KGGZCore::CoreClient::FeedbackMessage message, KGGZCore::Error::ErrorCode error)
{
	Q_UNUSED(error);

	switch(message)
	{
		case KGGZCore::CoreClient::connection:
			if(error == KGGZCore::Error::no_status)
				activity(i18n("Negotiating..."));
			else
				activity(i18n("Connection failed!"));
			break;
		case KGGZCore::CoreClient::negotiation:
			if(error == KGGZCore::Error::no_status)
				activity(i18n("Logging in..."));
			else
				activity(i18n("Negotiation failed!"));
			break;
		case KGGZCore::CoreClient::login:
			if(error == KGGZCore::Error::no_status)
				activity(i18n("Entering the room..."));
			else
				activity(i18n("Login failed!"));
			break;
		case KGGZCore::CoreClient::roomenter:
			activity(i18n("In the room %1!", m_core->room()->name()));
			connect(m_core->room(),
				SIGNAL(signalFeedback(KGGZCore::Room::FeedbackMessage, KGGZCore::Error::ErrorCode)),
				SLOT(slotFeedback(KGGZCore::Room::FeedbackMessage, KGGZCore::Error::ErrorCode)));
			connect(m_core->room(),
				SIGNAL(signalAnswer(KGGZCore::Room::AnswerMessage)),
				SLOT(slotAnswer(KGGZCore::Room::AnswerMessage)));
			connect(m_core->room(),
				SIGNAL(signalEvent(KGGZCore::Room::EventMessage)),
				SLOT(slotEvent(KGGZCore::Room::EventMessage)));
			break;
		case KGGZCore::CoreClient::chat:
			break;
		case KGGZCore::CoreClient::channel:
			break;
		case KGGZCore::CoreClient::logout:
			break;
	}
}

void PlasmaKGGZ::slotAnswer(KGGZCore::CoreClient::AnswerMessage message)
{
	int num;

	switch(message)
	{
		case KGGZCore::CoreClient::roomlist:
			num = m_core->roomnames().count();
			if(num > 0)
				m_core->initiateRoomChange(m_core->roomnames().at(num - 1));
			break;
		case KGGZCore::CoreClient::typelist:
			break;
		case KGGZCore::CoreClient::motd:
			break;
	}
}

void PlasmaKGGZ::slotEvent(KGGZCore::CoreClient::EventMessage message)
{
	switch(message)
	{
		case KGGZCore::CoreClient::state_changed:
			if(m_core->state() == KGGZCore::reconnecting)
				activity(i18n("Reconnecting..."));
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

void PlasmaKGGZ::slotFeedback(KGGZCore::Room::FeedbackMessage message, KGGZCore::Error::ErrorCode error)
{
	switch(message)
	{
		case KGGZCore::Room::tablelaunched:
			break;
		case KGGZCore::Room::tablejoined:
			break;
		case KGGZCore::Room::tableleft:
			break;
	}
}

void PlasmaKGGZ::slotAnswer(KGGZCore::Room::AnswerMessage message)
{
	switch(message)
	{
		case KGGZCore::Room::playerlist:
			break;
		case KGGZCore::Room::tablelist:
			qDebug() << "#tables";
			checkTables();
			break;
	}
}

void PlasmaKGGZ::slotEvent(KGGZCore::Room::EventMessage message)
{
	switch(message)
	{
		case KGGZCore::Room::chat:
			break;
		case KGGZCore::Room::enter:
			break;
		case KGGZCore::Room::leave:
			break;
		case KGGZCore::Room::tableupdate:
			qDebug() << "#tables(update)";
			checkTables();
			break;
		case KGGZCore::Room::playerlag:
			break;
		case KGGZCore::Room::stats:
			break;
		case KGGZCore::Room::count:
			break;
		case KGGZCore::Room::perms:
			break;
		case KGGZCore::Room::libraryerror:
			break;
	}
}
 
#include "plasma-kggz.moc"
