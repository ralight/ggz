#include <QtGui/QPushButton>
#include <QtGui/QLayout>
#include <QtGui/QListWidget>

#include <klocale.h>

#include "roomselector.h"

RoomSelector::RoomSelector(QWidget *parent)
: QDialog(parent)
{
	m_roomlist = new QListWidget();

	QListWidgetItem *item = new QListWidgetItem("Tic-Tac-Toe");
	m_roomlist->addItem(item);

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

void RoomSelector::setGGZUri(QString uri)
{
	Q_UNUSED(uri);
}

#include "roomselector.moc"
