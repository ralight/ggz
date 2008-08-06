#include <QtGui/QGridLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QComboBox>

#include <klocale.h>

#include "configwidget.h"
#include "roomselector.h"
#include "serverselector.h"

ConfigWidget::ConfigWidget(QWidget *parent, bool servereditable)
: QWidget(parent)
{
	QLabel *label;

	QGridLayout *layout = new QGridLayout();
	setLayout(layout);

	m_username = new QLineEdit();
	m_password = new QLineEdit();
	m_email = new QLineEdit();
	m_realname = new QLineEdit();
	m_roomname = new QLineEdit();
	m_ggzserver = new QLineEdit();
	m_password->setEchoMode(QLineEdit::Password);
	if(!servereditable)
	{
		m_ggzserver->setEnabled(false);
	}
	m_username->setEnabled(false);
	m_password->setEnabled(false);
	m_email->setEnabled(false);
	m_realname->setEnabled(false);
	m_roomname->setEnabled(false);

	m_logintype = new QComboBox();
	m_logintype->addItem("Guest", guest);
	m_logintype->addItem("First time", firsttime);
	m_logintype->addItem("Registered user", registered);
	m_logintype->setEnabled(false);

	label = new QLabel(i18n("GGZ Server:"));
	label->setBuddy(m_ggzserver);
	layout->addWidget(label, 0, 0);
	layout->addWidget(m_ggzserver, 0, 1);

	label = new QLabel(i18n("Login mode:"));
	label->setBuddy(m_logintype);
	layout->addWidget(label, 1, 0);
	layout->addWidget(m_logintype, 1, 1);

	label = new QLabel(i18n("Username:"));
	label->setBuddy(m_username);
	layout->addWidget(label, 2, 0);
	layout->addWidget(m_username, 2, 1);

	label = new QLabel(i18n("Password:"));
	label->setBuddy(m_password);
	layout->addWidget(label, 3, 0);
	layout->addWidget(m_password, 3, 1);

	label = new QLabel(i18n("Real name:"));
	label->setBuddy(m_realname);
	layout->addWidget(label, 4, 0);
	layout->addWidget(m_realname, 4, 1);

	label = new QLabel(i18n("E-mail address:"));
	label->setBuddy(m_email);
	layout->addWidget(label, 5, 0);
	layout->addWidget(m_email, 5, 1);

	label = new QLabel(i18n("Room name:"));
	label->setBuddy(m_roomname);
	layout->addWidget(label, 6, 0);
	layout->addWidget(m_roomname, 6, 1);

	QPushButton *serverbutton = NULL;
	if(servereditable)
	{
		serverbutton = new QPushButton(i18n("Select..."));
		layout->addWidget(serverbutton, 0, 2);
	}

	m_roombutton = new QPushButton(i18n("Select..."));
	m_roombutton->setEnabled(false);
	layout->addWidget(m_roombutton, 6, 2);

	if(serverbutton)
	{
		connect(serverbutton, SIGNAL(clicked()), SLOT(slotSelectServer()));
	}
	connect(m_roombutton, SIGNAL(clicked()), SLOT(slotSelectRoom()));
	connect(m_logintype, SIGNAL(currentIndexChanged(int)), SLOT(slotLoginType(int)));
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

void ConfigWidget::setRealname(const QString &realname)
{
	m_realname->setText(realname);
}

QString ConfigWidget::realname() const
{
	return m_realname->text();
}

void ConfigWidget::setEmail(const QString &email)
{
	m_email->setText(email);
}

QString ConfigWidget::email() const
{
	return m_email->text();
}

void ConfigWidget::setLoginType(LoginType type)
{
	int index = m_logintype->findData(type);
	m_logintype->setCurrentIndex(index);
}

ConfigWidget::LoginType ConfigWidget::loginType() const
{
	int type = m_logintype->itemData(m_logintype->currentIndex()).toInt();
	return (LoginType)type;
}

void ConfigWidget::setGGZServer(const GGZServer &server)
{
	m_ggzserver->setText(server.uri());

	m_server = server;

	bool enabled = !server.api().isEmpty();
	m_roombutton->setEnabled(enabled);
	m_roomname->setEnabled(enabled);
	m_username->setEnabled(enabled);
	m_password->setEnabled(enabled);
	m_email->setEnabled(enabled);
	m_realname->setEnabled(enabled);
	m_logintype->setEnabled(enabled);

	slotLoginType(-1);
}

GGZServer ConfigWidget::ggzServer() const
{
	return m_server;
}

void ConfigWidget::slotSelectRoom()
{
	RoomSelector selector(this);
	selector.setGGZApi(m_server.api());
	int status = selector.exec();
	if(status == QDialog::Accepted)
	{
		setRoomname(selector.room());
	}
}

void ConfigWidget::slotSelectServer()
{
	ServerSelector selector(this);
	selector.setMetaUri(m_metaserver);
	int status = selector.exec();
	if(status == QDialog::Accepted)
	{
		setGGZServer(selector.server());
	}
}

void ConfigWidget::setMetaserver(const QString &metaserver)
{
	m_metaserver = metaserver;
}

void ConfigWidget::slotLoginType(int index)
{
	Q_UNUSED(index);

	LoginType type = loginType();
	if(type == guest)
	{
		m_password->setEnabled(false);
		m_email->setEnabled(false);
		m_realname->setEnabled(false);
	}
	else if(type == firsttime)
	{
		m_password->setEnabled(true);
		m_email->setEnabled(true);
		m_realname->setEnabled(true);
	}
	else if(type == registered)
	{
		m_password->setEnabled(true);
		m_email->setEnabled(false);
		m_realname->setEnabled(false);
	}

	QString strtype;
	if(type == guest)
		strtype = "guest";
	else if(type == firsttime)
		strtype = "firsttime";
	else if(type == registered)
		strtype = "registered";

	m_server.setLoginType(strtype);
	emit signalChanged(m_server);
}

#include "configwidget.moc"
