#include <QtGui/QGridLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>

#include <klocale.h>

#include "configwidget.h"

ConfigWidget::ConfigWidget(QWidget *parent)
: QWidget(parent)
{
	QGridLayout *layout = new QGridLayout(this);

	m_username = new QLineEdit(this);
	m_password = new QLineEdit(this);

	QLabel *label = new QLabel(i18n("Username:"), this);
	label->setBuddy(m_username);
	layout->addWidget(label, 0, 0);
	layout->addWidget(m_username, 0, 1);

	label = new QLabel(i18n("Password:"), this);
	label->setBuddy(m_password);
	layout->addWidget(label, 1, 0);
	layout->addWidget(m_password, 1, 1);
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

#include "configwidget.moc"