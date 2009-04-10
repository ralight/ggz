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
	m_logintype->addItem("Guest", GGZProfile::guest);
	m_logintype->addItem("First time", GGZProfile::firsttime);
	m_logintype->addItem("Registered user", GGZProfile::registered);
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

	slotLoginType(m_logintype->findData(GGZProfile::guest));
}

ConfigWidget::~ConfigWidget()
{
}

void ConfigWidget::setGGZProfile(const GGZProfile &profile)
{
	m_profile = profile;
	GGZServer server = profile.ggzServer();

	m_ggzserver->setText(server.uri());

	m_username->setText(profile.username());
	m_password->setText(profile.password());
	m_roomname->setText(profile.roomname());
	m_realname->setText(profile.realname());
	m_email->setText(profile.email());
	int index = m_logintype->findData(profile.loginType());
	m_logintype->setCurrentIndex(index);

	bool apienabled = !server.api().isEmpty();
	m_roombutton->setEnabled(apienabled);

	bool enabled = !server.uri().isEmpty();
	m_roomname->setEnabled(enabled);
	m_username->setEnabled(enabled);
	m_logintype->setEnabled(enabled);
}

GGZProfile ConfigWidget::ggzProfile()
{
	m_profile.setUsername(m_username->text());
	m_profile.setPassword(m_password->text());
	m_profile.setRoomname(m_roomname->text());
	m_profile.setRealname(m_realname->text());
	m_profile.setEmail(m_email->text());
	int type = m_logintype->itemData(m_logintype->currentIndex()).toInt();
	m_profile.setLoginType((GGZProfile::LoginType)type);

	return m_profile;
}

void ConfigWidget::slotSelectRoom()
{
	RoomSelector selector(this);
	selector.setGGZApi(m_profile.ggzServer().api());
	int status = selector.exec();
	if(status == QDialog::Accepted)
	{
		m_profile.setRoomname(selector.room());
	}
}

void ConfigWidget::slotSelectServer()
{
	ServerSelector selector(this);
	selector.setMetaUri(m_metaserver);
	int status = selector.exec();
	if(status == QDialog::Accepted)
	{
		GGZProfile profile;
		profile.setGGZServer(selector.server());
		setGGZProfile(profile);
	}
}

void ConfigWidget::setMetaserver(const QString &metaserver)
{
	m_metaserver = metaserver;
}

void ConfigWidget::slotLoginType(int index)
{
	GGZProfile::LoginType type = (GGZProfile::LoginType)m_logintype->itemData(index).toInt();

	if(type == GGZProfile::guest)
	{
		m_password->setEnabled(false);
		m_email->setEnabled(false);
		m_realname->setEnabled(false);
	}
	else if(type == GGZProfile::firsttime)
	{
		m_password->setEnabled(true);
		m_email->setEnabled(true);
		m_realname->setEnabled(true);
	}
	else if(type == GGZProfile::registered)
	{
		m_password->setEnabled(true);
		m_email->setEnabled(false);
		m_realname->setEnabled(false);
	}

	m_profile.setLoginType(type),
	emit signalChanged(m_profile);
}

#include "configwidget.moc"
