#include <QtGui/QGridLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>

#include <klocale.h>

#include "configwidget.h"
#include "roomselector.h"
#include "serverselector.h"

ConfigWidget::ConfigWidget(QWidget *parent)
: QWidget(parent)
{
	QLabel *label;

	QGridLayout *layout = new QGridLayout();
	setLayout(layout);

	m_username = new QLineEdit();
	m_password = new QLineEdit();
	m_roomname = new QLineEdit();
	m_ggzserver = new QLineEdit();
	m_password->setEchoMode(QLineEdit::Password);

	label = new QLabel(i18n("GGZ Server:"));
	label->setBuddy(m_ggzserver);
	layout->addWidget(label, 0, 0);
	layout->addWidget(m_ggzserver, 0, 1);

	label = new QLabel(i18n("Username:"));
	label->setBuddy(m_username);
	layout->addWidget(label, 1, 0);
	layout->addWidget(m_username, 1, 1);

	label = new QLabel(i18n("Password:"));
	label->setBuddy(m_password);
	layout->addWidget(label, 2, 0);
	layout->addWidget(m_password, 2, 1);

	label = new QLabel(i18n("Roomname:"));
	label->setBuddy(m_roomname);
	layout->addWidget(label, 3, 0);
	layout->addWidget(m_roomname, 3, 1);

	QPushButton *serverbutton = new QPushButton(i18n("Select..."));
	layout->addWidget(serverbutton, 0, 2);

	QPushButton *roombutton = new QPushButton(i18n("Select..."));
	layout->addWidget(roombutton, 3, 2);

	connect(serverbutton, SIGNAL(clicked()), SLOT(slotSelectServer()));
	connect(roombutton, SIGNAL(clicked()), SLOT(slotSelectRoom()));
}

ConfigWidget::~ConfigWidget()
{
}

void ConfigWidget::setUsername(const QString &username)
{
	m_username->setText(username);
}

QString ConfigWidget::username() const
{
	return m_username->text();
}

void ConfigWidget::setPassword(const QString &password)
{
	m_password->setText(password);
}

QString ConfigWidget::password() const
{
	return m_password->text();
}

void ConfigWidget::setRoomname(const QString &roomname)
{
	m_roomname->setText(roomname);
}

QString ConfigWidget::roomname() const
{
	return m_roomname->text();
}

void ConfigWidget::setGGZServer(const GGZServer &server)
{
	m_ggzserver->setText(server.uri());
}

GGZServer ConfigWidget::ggzServer() const
{
	GGZServer server;
	server.setUri(m_ggzserver->text());
	return server;
}

void ConfigWidget::setGGZUri(const QString &uri)
{
	m_ggzserver->setText(uri);
}

QString ConfigWidget::ggzUri() const
{
	return m_ggzserver->text();
}

void ConfigWidget::slotSelectRoom()
{
	RoomSelector selector(this);
	selector.setGGZUri(m_ggzserver->text());
	int status = selector.exec();
	if(status == QDialog::Accepted)
	{
		setRoomname(selector.room());
	}
}

void ConfigWidget::slotSelectServer()
{
	ServerSelector selector(this);
	selector.setMetaUri("ggz://meta.ggzgamingzone.org:15689");
	int status = selector.exec();
	if(status == QDialog::Accepted)
	{
		m_ggzserver->setText(selector.server());
	}
}

#include "configwidget.moc"
