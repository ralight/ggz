#include "KGGZLaunch.h"

#include <qlayout.h>
#include <qlabel.h>
#include <qpushbutton.h>

#define SEAT_OPEN 1
#define SEAT_BOT 2
#define SEAT_RESERVED 3

#include "KGGZCommon.h"

KGGZLaunch::KGGZLaunch(QWidget *parent = NULL, char *name = NULL)
: QWidget(parent, name)
{
	QVBoxLayout *vbox;
	QHBoxLayout *hbox;
	QLabel *label1, *label2, *label3;
	QPushButton *ok, *cancel;

	KGGZDEBUGF("KGGZLaunch::KGGZLaunch()\n");
	m_popup = NULL;

	m_slider = new QSlider(this);
	m_slider->setOrientation(QSlider::Horizontal);
	m_slider->setMinValue(2);
	m_slider->setValue(2);
	m_slider->setMaxValue(8);
	m_slider->setTickInterval(1);
	m_slider->setTickmarks(QSlider::Below);

	m_listbox = new QListBox(this);
	m_listbox->insertItem("Seat 1: Bot");
	m_listbox->insertItem("Seat 2: Josef");
	m_listbox->insertItem("Seat 3: (unused)");
	m_listbox->insertItem("Seat 4: (unused)");
	m_listbox->insertItem("Seat 5: (unused)");
	m_listbox->insertItem("Seat 6: (unused)");
	m_listbox->insertItem("Seat 7: (unused)");
	m_listbox->insertItem("Seat 8: (unused)");

	m_edit = new QLineEdit(this);

	label1 = new QLabel("Number of players:", this);
	label2 = new QLabel("Seat assignments:", this);
	label3 = new QLabel("Game description:", this);

	ok = new QPushButton("OK", this);
	cancel = new QPushButton("Cancel", this);

	vbox = new QVBoxLayout(this, 5);
	vbox->add(label1);
	vbox->add(m_slider);
	vbox->add(label2);
	vbox->add(m_listbox);
	vbox->add(label3);
	vbox->add(m_edit);

	hbox = new QHBoxLayout(vbox, 5);
	hbox->add(ok);
	hbox->add(cancel);

	connect(m_listbox, SIGNAL(rightButtonPressed(QListBoxItem*, const QPoint&)), SLOT(slotSelected(QListBoxItem*, const QPoint&)));
	connect(m_slider, SIGNAL(valueChanged(int)), SLOT(slotChanged(int)));
	connect(ok, SIGNAL(clicked()), SLOT(slotAccepted()));
	connect(cancel, SIGNAL(clicked()), SLOT(close()));

	setFixedSize(250, 300);
	setCaption("Launch a game");
	show();
}

KGGZLaunch::~KGGZLaunch()
{
}

void KGGZLaunch::slotSelected(QListBoxItem *selected, const QPoint& point)
{
	if(!selected) return;
	if(!selected->isSelectable()) return;

	if(m_popup) delete m_popup;

	m_popup = new QPopupMenu(this);
	m_popup->insertItem("Bot", SEAT_BOT);
	m_popup->insertItem("Reserved", SEAT_RESERVED);
	m_popup->insertItem("Open", SEAT_OPEN);
	m_popup->popup(point);

	connect(m_popup, SIGNAL(activated(int)), SLOT(slotActivated(int)));
}

void KGGZLaunch::slotActivated(int id)
{
	int i;
	char buffer[128];

	i = m_listbox->currentItem();
	if(i == -1) return;

	switch(id)
	{
		case SEAT_BOT:
			sprintf(buffer, "Seat %i: Bot", i + 1);
			break;
		case SEAT_RESERVED:
			sprintf(buffer, "Seat %i: Reserved", i + 1);
			break;
		case SEAT_OPEN:
			sprintf(buffer, "Seat %i: Open", i + 1);
			break;
	}
	m_listbox->changeItem(buffer, i);
}

void KGGZLaunch::slotAccepted()
{
	emit signalLaunch();
}

void KGGZLaunch::slotChanged(int value)
{
	QListBoxItem *tmp;

	for(int i = 0; i < 8; i++)
	{
		tmp = m_listbox->item(i);
		if(i < value) tmp->setSelectable(TRUE);
		else tmp->setSelectable(FALSE);
		//m_listbox->changeItem(tmp, i);
	}
}

const char *KGGZLaunch::description()
{
	return m_edit->text().latin1();
}

int KGGZLaunch::seats()
{
	return m_slider->value();
}
