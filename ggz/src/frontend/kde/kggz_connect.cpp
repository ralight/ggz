/* Class declaration */
#include "kggz_connect.h"

/* Abstraction of GGZ Core client functions */
#include "kggz_server.h"

/* GGZcore for server profiles */ // implicit! already included by kggz_server
#include <ggzcore.h>

/* KGGZ classes */
#include "kggz_hosts.h"
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
#include <qradiobutton.h>
#include <qcheckbox.h>
#include <qbuttongroup.h>

/* System includes */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

QLineEdit *input_host, *input_port, *input_name, *input_password;
#ifdef USE_SERVER
QCheckBox *option_server;
#endif
int m_connected2;
QComboBox *profile_select;

/* Constructor: set up a small dialog for connections; provide server profile list */
KGGZ_Connect::KGGZ_Connect(QWidget *parent, char *name)
: QWidget(parent, name)
{
	QVBoxLayout *vbox1;
	QHBoxLayout *hbox1, *hbox2, *hbox3;

	QPushButton *button_ok, *button_cancel;
	QLabel *label_host, *label_port, *label_name, *label_password;
	QPushButton *profile_edit;
	QRadioButton *mode_normal, *mode_guest, *mode_firsttime;
	QLabel *label_mode;
	QButtonGroup *group_mode;

	profile_select = new QComboBox(this);
	//profile_select->insertItem(i18n("Default server"));

	profile_edit = new QPushButton(i18n("Edit Profiles"), this);

	label_host = new QLabel(i18n("Server host:"), this);
	label_port = new QLabel(i18n("Port:"), this);
	label_name = new QLabel(i18n("Log in as user:"), this);
	label_password = new QLabel(i18n("Use password:"), this);

	input_host = new QLineEdit(this);
	input_port = new QLineEdit(this);
	input_name = new QLineEdit(this);
	input_password = new QLineEdit(this);

	//input_host->setText("localhost");
	//input_port->setText("5688");
	//input_name->setText("gambler");
	//input_password->setText("secret");
	input_password->setEchoMode(QLineEdit::Password);

	button_ok = new QPushButton("OK", this);
	button_cancel = new QPushButton(i18n("Cancel"), this);

	label_mode = new QLabel(i18n("Log in as:"), this);
	group_mode = new QButtonGroup(this);
	group_mode->hide();
	mode_normal = new QRadioButton(i18n("normal"), this);
	mode_guest = new QRadioButton(i18n("guest"), this);
	mode_firsttime = new QRadioButton(i18n("starter"), this);
	group_mode->insert(mode_normal);
	group_mode->insert(mode_guest);
	group_mode->insert(mode_firsttime);

	// hm, perhaps an extra slot for that
	mode_normal->setChecked(TRUE);
	m_loginmode = 0;

#ifdef USE_SERVER
	option_server = new QCheckBox(i18n("Start ggzd server locally"), this);
	//option_server->setChecked(TRUE); to be user configurable
	//input_host->setEnabled(FALSE); and that's the consequence
#endif

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
	vbox1->add(label_password);
	vbox1->add(input_password);

	hbox3 = new QHBoxLayout(vbox1, 5);
	hbox3->add(label_mode);
	//hbox3->add(group_mode);
	hbox3->add(mode_normal);
	hbox3->add(mode_guest);
	hbox3->add(mode_firsttime);

#ifdef USE_SERVER
	vbox1->add(option_server);
#endif

	hbox1 = new QHBoxLayout(vbox1, 5);
	hbox1->add(button_ok);
	hbox1->add(button_cancel);

	connect(profile_edit, SIGNAL(clicked()), SLOT(edit()));
	connect(button_cancel, SIGNAL(clicked()), SLOT(close()));
	connect(button_ok, SIGNAL(clicked()), SLOT(accept()));
	connect(group_mode, SIGNAL(clicked(int)), SLOT(modes(int)));
#ifdef USE_SERVER
	connect(option_server, SIGNAL(clicked()), SLOT(invoke()));
#endif

	m_connected2 = -1;

	setCaption(i18n("Connect to server"));

	resize(300, 260);

	// ggzcore_conf
	slotLoadProfile(-1);
	connect(profile_select, SIGNAL(activated(int)), SLOT(slotLoadProfile(int)));
}

/* Destructor */
KGGZ_Connect::~KGGZ_Connect()
{
}

/* Loads a specified server profile, or the first if profile is -1 */
void KGGZ_Connect::slotLoadProfile(int profile)
{
	char **list;
	int i;
	char *host;
	char *port;
	char *username;
	char *password;
	int type;
	char *listentry;

	// setup entries; try to find suitable first entry
	if(profile == -1)
	{
		profile_select->clear();
		ggzcore_conf_read_list("Servers", "Servers", &i, &list);
		for(int j = 0; j < i; j++)
		{
			if(j == 0)
			{
				listentry = (char*)malloc(strlen(list[j]) + 1);
				strcpy(listentry, list[j]);
			}
			profile_select->insertItem(list[j]);
			free(list[j]);
		}
		free(list);
		if(i == 0)
		{
			listentry = (char*)malloc(512);
			strcpy(listentry, i18n("Default server"));
		}
	}
	else
	{
		listentry = (char*)malloc(strlen(profile_select->text(profile)) + 1);
		strcpy(listentry, profile_select->text(profile));
	}

	// read values for selected server
	host = ggzcore_conf_read_string(listentry, "Host", NULL);
	port = ggzcore_conf_read_string(listentry, "Port", "5688");
	username = ggzcore_conf_read_string(listentry, "Login", "Zocker");
	password = ggzcore_conf_read_string(listentry, "Password", "");
	type = ggzcore_conf_read_int(listentry, "Type", 0);

	// put values into fields
	input_host->setText(host);
	input_port->setText(port);
	input_name->setText(username);
	input_password->setText(password);

	// free allocated memory
	free(host);
	free(port);
	free(username);
	free(password);
	free(listentry);
}

/* Start a connection */
void KGGZ_Connect::accept()
{
	KGGZ_Motd *motd;
	int result;

#ifdef USE_SERVER
	if(option_server->isChecked())
	{
		result = KGGZ_Server::launchServer();
		if(result == -1) KMessageBox::error(this, i18n("Could not start ggzd!"), "Error!");
		if(result == -2) KMessageBox::error(this, i18n("The ggzd server is already running!"), "Error!");
	}
#endif

	KGGZ_Server::connect(input_host->text(), atoi(input_port->text()), input_name->text(), input_password->text(), m_loginmode);
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
	KGGZ_Hosts *hosts;

	hosts = new KGGZ_Hosts(NULL, NULL);
	hosts->show();
	connect(hosts, SIGNAL(signalUpdate(int)), SLOT(slotLoadProfile(int)));
}

void KGGZ_Connect::modes(int id)
{
	if(id == 1) input_password->setEnabled(FALSE);
	else input_password->setEnabled(TRUE);
	m_loginmode = id;
}

void KGGZ_Connect::invoke()
{
// yeah... as you already had in mind, the moc is stupid enough...
#ifdef USE_SERVER
	if(input_host->isEnabled())
	{
		input_host->setEnabled(FALSE);
		profile_select->setEnabled(FALSE);
	}
	else
	{
		input_host->setEnabled(TRUE);
		profile_select->setEnabled(TRUE);
	}
#endif
}
