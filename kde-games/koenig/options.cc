#include "options.h"

#include <qlayout.h>
#include <qradiobutton.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qspinbox.h>
#include <qbuttongroup.h>

Options::Options(QWidget *parent, const char *name)
: QWidget(parent, name)
{
	QVBoxLayout *vbox, *vbox2;
	QHBoxLayout *hbox;
	QLabel *heading, *text, *timetext, *labelminutes, *labelseconds;
	QPushButton *ok;
	QButtonGroup *group;

	group = new QButtonGroup("Selection", this);

	noclock = new QRadioButton("No clock", group);
	noclock->setChecked(true);
	serverclock = new QRadioButton("Server clock", group);
	clientclock = new QRadioButton("Client clock", group);

	heading = new QLabel("Chess game options", this);
	text = new QLabel("Select which clock model to use for this game.", this);
	timetext = new QLabel("How long is one turn?", this);
	ok = new QPushButton("Submit settings", this);

	minutes = new QSpinBox(0, 30, 1, this);
	minutes->setValue(15);
	seconds = new QSpinBox(0, 30, 1, this);

	labelminutes = new QLabel("Minutes:", this);
	labelseconds = new QLabel("Seconds:", this);

	vbox2 = new QVBoxLayout(group, 5);
	vbox2->addSpacing(10);
	vbox2->add(noclock);
	vbox2->add(serverclock);
	vbox2->add(clientclock);

	vbox = new QVBoxLayout(this, 5);
	vbox->add(heading);
	vbox->add(text);
	vbox->addSpacing(10);
	vbox->add(group);
	vbox->addSpacing(10);
	vbox->add(timetext);
	hbox = new QHBoxLayout(vbox, 5);
	hbox->add(labelminutes);
	hbox->add(minutes);
	hbox->add(labelseconds);
	hbox->add(seconds);
	vbox->addSpacing(10);
	vbox->add(ok);

	connect(ok, SIGNAL(clicked()), SLOT(slotTime()));

	setCaption("Option");
	show();
}

Options::~Options()
{
}

void Options::slotTime()
{
	int time;

	time = minutes->cleanText().toInt() + seconds->cleanText().toInt();

	if(noclock->isChecked())
		emit signalTime(0, 0);
	else if(serverclock->isChecked()) // add lag option (gtk+ client too)!!!
		emit signalTime(3, time);
	else
		emit signalTime(1, time);
}

