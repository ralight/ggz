#include <QtGui/QPushButton>
#include <QtGui/QLayout>
#include <QtGui/QListWidget>

#include <qdom.h>

#include <klocale.h>
#include <kio/job.h>
#include <kmessagebox.h>

#include "roomselector.h"

RoomSelector::RoomSelector(QWidget *parent)
: QDialog(parent)
{
	m_roomlist = new QListWidget();

	m_button = new QPushButton(i18n("Select this room"));
	m_button->setEnabled(false);

	QPushButton *cancel = new QPushButton(i18n("Cancel"));

	QHBoxLayout *hbox = new QHBoxLayout();
	hbox->addWidget(m_button);
	hbox->addWidget(cancel);

	QVBoxLayout *vbox = new QVBoxLayout();
	setLayout(vbox);
	vbox->addWidget(m_roomlist);
	vbox->addLayout(hbox);

	connect(m_button, SIGNAL(clicked()), SLOT(slotRoomSelected()));
	connect(cancel, SIGNAL(clicked()), SLOT(reject()));
	connect(m_roomlist, SIGNAL(itemSelectionChanged()), SLOT(slotSelectionChanged()));

	setWindowTitle(i18n("Room selection from GGZ server"));
	show();
}

RoomSelector::~RoomSelector()
{
}

QString RoomSelector::room()
{
	QListWidgetItem *item = m_roomlist->currentItem();

	if(item)
		return item->text();

	return QString();
}

void RoomSelector::slotRoomSelected()
{
	QListWidgetItem *item = m_roomlist->currentItem();

	if(item)
	{
		emit signalRoomSelected(item->text());
		accept();
	}
}

void RoomSelector::slotSelectionChanged()
{
	QListWidgetItem *item = m_roomlist->currentItem();

	m_button->setEnabled((item != NULL));
}

void RoomSelector::setGGZApi(QString uri)
{
	m_data.clear();

	if(uri.endsWith("/"))
		uri.chop(1);
	uri += "/rooms";

	KIO::TransferJob *job = KIO::get(KUrl(uri), KIO::NoReload, KIO::HideProgressInfo);
	connect(job, SIGNAL(data(KIO::Job*, const QByteArray&)), SLOT(slotData(KIO::Job*, const QByteArray&)));
	connect(job, SIGNAL(result(KJob*)), SLOT(slotResult(KJob*)));
}

void RoomSelector::slotData(KIO::Job *job, const QByteArray &data)
{
	Q_UNUSED(job);

	m_data.append(data);
}

void RoomSelector::slotResult(KJob *job)
{
	if(job->error())
	{
		KMessageBox::error(this,
			i18n("The list of rooms cannot be retrieved."),
			i18n("Web Service API failure"));
		return;
	}
	//qDebug("XML: %s", m_data.data());

	QDomDocument dom;
	if(!dom.setContent(m_data))
	{
		KMessageBox::error(this,
			i18n("The list of rooms is malformatted."),
			i18n("Web Service API failure"));
		return;
	}

	QDomElement rooms = dom.documentElement();
	QDomNode roomnode = rooms.firstChild();
	while(!roomnode.isNull())
	{
		QDomElement room = roomnode.toElement();
		QString name = room.attribute("name");

		QListWidgetItem *item = new QListWidgetItem(name);
		m_roomlist->addItem(item);

		roomnode = roomnode.nextSibling();
	}
}

#include "roomselector.moc"
