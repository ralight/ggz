#include "kggz_teams.h"

#include <qlayout.h>
#include <qpushbutton.h>
#include <qlistbox.h>
#include <qlabel.h>

KGGZ_Teams::KGGZ_Teams(QWidget *parent, char *name)
{
	QHBoxLayout *hbox;
	QVBoxLayout *vbox1, *vbox2, *vboxc;
	QListBox *list1, *list2;
	QPushButton *to1, *to2, *done;
	QLabel *label1, *label2;

	hbox = new QHBoxLayout(this, 5);

	vbox1 = new QVBoxLayout(hbox,5);

	label1 = new QLabel("Team 1", this);
	list1 = new QListBox(this);
	list1->insertItem("Player 1");
	list1->insertItem("Player 2");
	vbox1->add(label1);
	vbox1->add(list1);

	vboxc = new QVBoxLayout(hbox, 5);

	to1 = new QPushButton("<--", this);
	to2 = new QPushButton("-->", this);
	done = new QPushButton("Done", this);
	vboxc->add(to1);
	vboxc->add(to2);
	vboxc->add(done);

	vbox2 = new QVBoxLayout(hbox,5);

	label2 = new QLabel("Team 2", this);
	list2 = new QListBox(this);
	vbox2->add(label2);
	vbox2->add(list2);

	connect(done, SIGNAL(clicked()), SLOT(done()));

	setFixedSize(300, 150);
	setCaption("Team Assignments");
	show();
}

KGGZ_Teams::~KGGZ_Teams()
{
}

void KGGZ_Teams::done()
{
	close();
}