/* Class declaration */
#include "kggz_profiles.h"

/* Qt classes */
#include <qlayout.h>
#include <qcombobox.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qlistview.h>

QListView *profiles_list;

KGGZ_Profiles::KGGZ_Profiles(QWidget *parent, char *name)
: QWidget(parent, name)
{
	QVBoxLayout *vbox1, *vbox2, *vbox3, *vbox4, *vbox5;
	QHBoxLayout *hbox1, *hbox2, *hbox3;

	QPushButton *button_ok, *button_cancel;
	QLabel *label_host, *label_port, *label_name;
	QPushButton *profile_add, *profile_remove;

	profiles_list = new QListView(this);
	profiles_list->addColumn("Description");
	profiles_list->addColumn("Host");
	profiles_list->addColumn("Port");

	profile_add = new QPushButton("Add", this);
	profile_remove = new QPushButton("Remove", this);

	label_name = new QLabel("Description:", this);
	label_host = new QLabel("Server host:", this);
	label_port = new QLabel("Port:", this);

	input_name = new QLineEdit(this);
	input_host = new QLineEdit(this);
	input_port = new QLineEdit(this);

	button_ok = new QPushButton("OK", this);
	button_cancel = new QPushButton("Cancel", this);

	vbox1 = new QVBoxLayout(this, 5);

	hbox1 = new QHBoxLayout(vbox1, 5);
	hbox1->add(profiles_list);

	vbox2 = new QVBoxLayout(hbox1, 5);
	vbox2->add(profile_add);
	vbox2->add(profile_remove);

	hbox2 = new QHBoxLayout(vbox1, 5);

	vbox3 = new QVBoxLayout(hbox2, 5);
	vbox3->add(label_host);
	vbox3->add(input_host);

	vbox4 = new QVBoxLayout(hbox2, 5);
	vbox4->add(label_port);
	vbox4->add(input_port);

	vbox5 = new QVBoxLayout(hbox2, 5);
	vbox5->add(label_name);
	vbox5->add(input_name);

	hbox3 = new QHBoxLayout(vbox1, 5);
	hbox3->add(button_ok);
	hbox3->add(button_cancel);

	connect(profile_add, SIGNAL(clicked()), SLOT(slotAddProfile()));
	connect(profile_remove, SIGNAL(clicked()), SLOT(slotRemoveProfile()));
	connect(button_cancel, SIGNAL(clicked()), SLOT(close()));
	connect(button_ok, SIGNAL(clicked()), SLOT(accept()));

	setCaption("Server profiles");
	resize(400, 200);
}

/* Destructor */
KGGZ_Profiles::~KGGZ_Profiles()
{
}

void KGGZ_Profiles::accept()
{
	close();
}

void KGGZ_Profiles::slotRemoveProfile()
{
	profiles_list->takeItem(profiles_list->currentItem());
}

void KGGZ_Profiles::slotAddProfile()
{
	profiles_list->insertItem(new QListViewItem(profiles_list, input_name->text(), input_host->text(), input_port->text()));
	input_name->clear();
	input_host->clear();
	input_port->clear();
}
