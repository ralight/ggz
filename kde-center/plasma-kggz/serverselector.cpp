#include <QtGui/QPushButton>
#include <QtGui/QLayout>
#include <QtGui/QListWidget>

#include <klocale.h>

#include "serverselector.h"

ServerSelector::ServerSelector(QWidget *parent)
: QDialog(parent)
{
	m_serverlist = new QListWidget();

	QListWidgetItem *item = new QListWidgetItem("ggz://live.ggzgamingzone.org");
	m_serverlist->addItem(item);

	m_button = new QPushButton(i18n("Select this server"));
	m_button->setEnabled(false);

	QPushButton *cancel = new QPushButton(i18n("Cancel"));

	QHBoxLayout *hbox = new QHBoxLayout();
	hbox->addWidget(m_button);
	hbox->addWidget(cancel);

	QVBoxLayout *vbox = new QVBoxLayout();
	setLayout(vbox);
	vbox->addWidget(m_serverlist);
	vbox->addLayout(hbox);

	connect(m_button, SIGNAL(clicked()), SLOT(slotServerSelected()));
	connect(cancel, SIGNAL(clicked()), SLOT(reject()));
	connect(m_serverlist, SIGNAL(itemSelectionChanged()), SLOT(slotSelectionChanged()));

	setWindowTitle(i18n("Server selection from metaserver"));
	show();
}

ServerSelector::~ServerSelector()
{
}

QString ServerSelector::server()
{
	QListWidgetItem *item = m_serverlist->currentItem();

	if(item)
		return item->text();

	return QString();
}

void ServerSelector::slotServerSelected()
{
	QListWidgetItem *item = m_serverlist->currentItem();

	if(item)
	{
		emit signalServerSelected(item->text());
		accept();
	}
}

void ServerSelector::slotSelectionChanged()
{
	QListWidgetItem *item = m_serverlist->currentItem();

	m_button->setEnabled((item != NULL));
}

void ServerSelector::setMetaUri(QString uri)
{
	Q_UNUSED(uri);
}

#include "serverselector.moc"
