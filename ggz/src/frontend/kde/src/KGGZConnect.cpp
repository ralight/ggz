// Class declaration
#include "KGGZConnect.h"

// KDE classes
#include <kmessagebox.h>
#include <klocale.h>

// GGZ includes
#include <GGZCoreConfio.h>

// KGGZ includes
#include "KGGZCommon.h"

// Qt classes
#include <qlayout.h>
#include <qlabel.h>
#include <qradiobutton.h>
#include <qvalidator.h>

// System includes
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

/* Constructor: set up a small dialog for connections; provide server profile list */
KGGZConnect::KGGZConnect(QWidget *parent, char *name)
: QWidget(parent, name, WStyle_Customize | WStyle_Tool | WStyle_DialogBorder)
{
	QVBoxLayout *vbox1;
	QHBoxLayout *hbox1, *hbox2, *hbox3;
	QPushButton *button_ok, *button_cancel;
	QLabel *label_host, *label_port, *label_name, *label_password;
	QRadioButton *mode_normal, *mode_guest, *mode_firsttime;
	QLabel *label_mode;
	QIntValidator *valid;

	KGGZDEBUGF("KGGZConnect::KGGZConnect()\n");

	profile_select = new QComboBox(this);
	profile_new = new QPushButton(i18n("New Profiles"), this);
	profile_delete = new QPushButton(i18n("Delete"), this);

	label_host = new QLabel(i18n("Server host:"), this);
	label_port = new QLabel(i18n("Port:"), this);
	label_name = new QLabel(i18n("Log in as user:"), this);
	label_password = new QLabel(i18n("Use password:"), this);

	valid = new QIntValidator(this);
	valid->setRange(1024, 32767);

	input_host = new QLineEdit(this);
	input_port = new QLineEdit(this);
	input_port->setValidator(valid);
	input_name = new QLineEdit(this);
	input_password = new QLineEdit(this);
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

	mode_normal->setChecked(TRUE);

	option_server = new QCheckBox(i18n("Start ggzd server locally"), this);

	vbox1 = new QVBoxLayout(this, 5);

	hbox2 = new QHBoxLayout(vbox1, 5);
	hbox2->add(profile_select);
	hbox2->add(profile_new);
	hbox2->add(profile_delete);

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
	hbox3->add(mode_normal);
	hbox3->add(mode_guest);
	hbox3->add(mode_firsttime);

	vbox1->add(option_server);

	hbox1 = new QHBoxLayout(vbox1, 5);
	hbox1->add(button_ok);
	hbox1->add(button_cancel);

	//connect(profile_edit, SIGNAL(clicked()), SLOT(edit()));
	connect(button_cancel, SIGNAL(clicked()), SLOT(close()));
	connect(button_ok, SIGNAL(clicked()), SLOT(slotAccept()));
	connect(group_mode, SIGNAL(clicked(int)), SLOT(slotModes(int)));
	connect(option_server, SIGNAL(clicked()), SLOT(slotInvoke()));
	connect(profile_select, SIGNAL(activated(int)), SLOT(slotLoadProfile(int)));

	m_connected = -1;
	m_loginmode = 0;

	setCaption(i18n("Connect to server"));
	resize(300, 260);

	KGGZDEBUGF("KGGZConnect::KGGZConnect() ready\n");

	slotLoadProfile(-1);
}

/* Destructor */
KGGZConnect::~KGGZConnect()
{
	KGGZDEBUGF("KGGZConnect::~KGGZConnect()\n");
}

void KGGZConnect::slotSaveProfile()
{
	GGZCoreConfio *config;
	char *defaultserver;

	KGGZDEBUGF("KGGZConnect::slotSaveProfile()\n");

	config = new GGZCoreConfio(KGGZCommon::append(getenv("HOME"), "/.ggz/kggz.rc"), GGZCoreConfio::readwrite | GGZCoreConfio::create);
	KGGZCommon::clear();

	defaultserver = config->read("Session", "Defaultserver", i18n("Default server"));

	config->write(defaultserver, "Host", input_host->text());
	config->write(defaultserver, "Port", input_port->text());
	config->write(defaultserver, "Login", input_name->text());
	config->write(defaultserver, "Password", input_password->text());
	config->write(defaultserver, "Type", m_loginmode);
	config->write("Session", "Defaultserver", profile_select->currentText());
	config->commit();
	delete config;

	KGGZDEBUGF("KGGZConnect::slotSaveProfile() ready\n");
}

/* Loads a specified server profile, or the first if profile is -1 */
void KGGZConnect::slotLoadProfile(int profile)
{
	GGZCoreConfio *config;
	char **list = NULL;
	int i;
	char *host = NULL;
	char *port = NULL;
	char *username = NULL;
	char *password = NULL;
	int type;
	const char *listentry = NULL;

	KGGZDEBUGF("KGGZConnect::slotLoadProfile()\n");

	if(profile != -1) slotSaveProfile();

	config = new GGZCoreConfio(KGGZCommon::append(getenv("HOME"), "/.ggz/kggz.rc"), GGZCoreConfio::readonly);
	KGGZCommon::clear();

	/*profile_edit->setEnabled(TRUE);*/

	// setup entries; try to find suitable first entry
	if(profile == -1)
	{
		profile_select->clear();
		listentry = config->read("Session", "Defaultserver", i18n("Default server"));
		profile_select->insertItem(listentry);
		KGGZDEBUG("Cleared profile_select. Rereading entries.\n");
		config->read("Servers", "Servers", &i, &list);
		KGGZDEBUG("Done. Now putting them in place. Found %i entries.\n", i);
		for(int j = 0; j < i; j++)
		{
			if(strcmp(list[j], listentry) != 0)
				profile_select->insertItem(list[j]);
			free(list[j]);
		}
		if(list)
		{
			KGGZDEBUG("List exists, deleting it.\n");
			free(list);
		}
		if(i == 0)
		{
			KGGZDEBUG("None found, using default server.\n");
			/*profile_select->insertItem(i18n("Default server"));*/
		}
	}
	else
		listentry = profile_select->text(profile);

	// read values for selected server
	KGGZDEBUG("slotLoadProfile: read values in %s...\n", listentry);
	host = config->read(listentry, "Host", "ggz.morat.net");
	port = config->read(listentry, "Port", "5688");
	username = config->read(listentry, "Login", "zocker");
	password = config->read(listentry, "Password", "");
	type = config->read(listentry, "Type", 0);

	// put values into fields
	KGGZDEBUG("slotLoadProfile: set text...\n");
	input_host->setText(host);
	input_port->setText(port);
	input_name->setText(username);
	input_password->setText(password);
	slotModes(type);

	// free allocated memory
	KGGZDEBUG("slotLoadProfile: Freeing memory...\n");
	if(host) free(host);
	if(port) free(port);
	if(username) free(username);
	if(password) free(password);
	//if(listentry) free(listentry);

	// get rid of the configuration object
	delete config;

	KGGZDEBUGF("KGGZConnect::slotLoadProfile() ready\n");
}

/* Start a connection */
void KGGZConnect::slotAccept()
{
	slotSaveProfile();
	emit signalConnect(input_host->text().latin1(), atoi(input_port->text().latin1()), input_name->text().latin1(), input_password->text().latin1(), m_loginmode, option_server->isChecked());
}

void KGGZConnect::slotModes(int loginmode)
{
	m_loginmode = loginmode;
	group_mode->setButton(loginmode);
	if(loginmode == 0)
		input_password->setEnabled(TRUE);
	else
		input_password->setEnabled(FALSE);
}

void KGGZConnect::slotInvoke()
{
	if(option_server->isChecked())
		input_host->setEnabled(FALSE);
	else
		input_host->setEnabled(TRUE);
}
