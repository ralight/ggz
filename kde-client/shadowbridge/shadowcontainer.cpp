#include "shadowcontainer.h"

#include <qlayout.h>
#include <qdatetime.h>
#include <qpixmap.h>
#include <qlabel.h>

#include "config.h"

ShadowContainer::ShadowContainer(QWidget *parent, const char *name)
: QWidget(parent, name)
{
	QVBoxLayout *vbox;
	QLabel *label;

	view = new KListView(this);
	view->setSorting(0, TRUE);
	view->addColumn("Time");
	view->addColumn("Event");
	view->addColumn("Data");

	label = new QLabel("Active client:", this);

	combo = new KComboBox(this);

	vbox = new QVBoxLayout(this, 5);
	vbox->add(label);
	vbox->add(combo);
	vbox->add(view);
}

ShadowContainer::~ShadowContainer()
{
}

void ShadowContainer::slotIncoming(const char *data)
{
	addEntry(data, KGGZ_DIRECTORY "/shadowbridge/incoming.png");
}

void ShadowContainer::slotOutgoing(const char *data)
{
	addEntry(data, KGGZ_DIRECTORY "/shadowbridge/outgoing.png");
}

void ShadowContainer::slotAdmin(const char *data)
{
	addEntry(data, KGGZ_DIRECTORY "/shadowbridge/admin.png");
	combo->insertItem(data);
}

void ShadowContainer::addEntry(const char *data, const char *pixmap)
{
	KListViewItem *tmp;

	tmp = new KListViewItem(view);
	tmp->setText(0, QTime::currentTime().toString());
	tmp->setPixmap(1, QPixmap(pixmap));
	tmp->setText(2, data);
}

