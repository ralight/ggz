/* Class declaration */
#include "kggz_connect.h"

/* Abstraction of GGZ Core client functions */
#include "kggz_server.h"

/* KGGZ classes */
#include "kggz_profiles.h"
#include "kggz_motd.h"

/* KDE classes */
#include <kmessagebox.h>
#include <klocale.h>

/* Qt classes */
#include <qlayout.h>
#include <qcombobox.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qlineedit.h>

/* System includes */
#include <stdlib.h>
#include <stdio.h>

QLineEdit *input_host, *input_port, *input_name;
int m_connected2;

/* Constructor: set up a small dialog for connections; provide server profile list */
KGGZ_Connect::KGGZ_Connect(QWidget *parent, char *name)
: QWidget(parent, name)
{
	QVBoxLayout *vbox1;
	QHBoxLayout *hbox1, *hbox2;

	QPushButton *button_ok, *button_cancel;
	QLabel *label_host, *label_port, *label_name;
	QComboBox *profile_select;
	QPushButton *profile_edit;

	profile_select = new QComboBox(this);
	profile_select->insertItem(i18n("Default server"));

	profile_edit = new QPushButton(i18n("Edit Profiles"), this);

	label_host = new QLabel(i18n("Server host:"), this);
	label_port = new QLabel(i18n("Port:"), this);
	label_name = new QLabel(i18n("Log in as user:"), this);

	input_host = new QLineEdit(this);
	input_port = new QLineEdit(this);
	input_name = new QLineEdit(this);

	input_host->setText("localhost");
	input_port->setText("5688");
	input_name->setText("gambler");

	button_ok = new QPushButton("OK", this);
	button_cancel = new QPushButton(i18n("Cancel"), this);

	vbox1 = new QVBoxLayout(this, 5);

	hbox2 = new QHBoxLayout(vbox1, 5);
	hbox2->add(profile_select);
	hbox2->add(profile_edit);

	vbox1->add(label_host);
	vbox1->add(input_host);
	vbox1->add(label_port);
	vbox1->add(input_port);
	vbox1->add(label_name);
	vbox1->add(input_name);

	hbox1 = new QHBoxLayout(vbox1, 5);
	hbox1->add(button_ok);
	hbox1->add(button_cancel);

	connect(profile_edit, SIGNAL(clicked()), SLOT(edit()));
	connect(button_cancel, SIGNAL(clicked()), SLOT(close()));
	connect(button_ok, SIGNAL(clicked()), SLOT(accept()));

	m_connected2 = -1;

	setCaption(i18n("Connect to server"));

	resize(300, 200);
}

/* Destructor */
KGGZ_Connect::~KGGZ_Connect()
{
}

/* Start a connection */
void KGGZ_Connect::accept()
{
	KGGZ_Motd *motd;

	KGGZ_Server::connect(input_host->text(), atoi(input_port->text()), input_name->text());
	while(m_connected2 == -1) KGGZ_Server::loop();
	if(!m_connected2)
	{
		m_connected2 = -1;
		KMessageBox::error(this, i18n("Could not connect to the server!"), "Error!");
	}
	else
	{
		m_connected2 = -1;
		motd = new KGGZ_Motd(NULL, "motd");
		close();
	}
}

/* Query event */
void KGGZ_Connect::success(int connection)
{
	printf("kggz_connect::success: %i\n", connection);
	if(connection) m_connected2 = 1;
	else m_connected2 = 0;
}

/* Show "edit profiles" dialog */
void KGGZ_Connect::edit()
{
	KGGZ_Profiles *profiles;

	profiles = new KGGZ_Profiles(NULL, NULL);
	profiles->show();
}