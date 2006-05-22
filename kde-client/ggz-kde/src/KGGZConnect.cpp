/////////////////////////////////////////////////////////////////////////////////////
//                                                                                 //
//    KGGZ - The KDE client for the GGZ Gaming Zone - Version 0.0.5pre             //
//    Copyright (C) 2000 - 2002 Josef Spillner - dr_maux@users.sourceforge.net     //
//    The MindX Open Source Project - http://mindx.sourceforge.net                 //
//    Published under GNU GPL conditions - view COPYING for details                //
//                                                                                 //
/////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////
//                                                                                 //
//    This program is free software; you can redistribute it and/or modify         //
//    it under the terms of the GNU General Public License as published by         //
//    the Free Software Foundation; either version 2 of the License, or            //
//    (at your option) any later version.                                          //
//                                                                                 //
//    This program is distributed in the hope that it will be useful,              //
//    but WITHOUT ANY WARRANTY; without even the implied warranty of               //
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                //
//    GNU General Public License for more details.                                 //
//                                                                                 //
//    You should have received a copy of the GNU General Public License            //
//    along with this program; if not, write to the Free Software                  //
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA    //
//                                                                                 //
/////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////
//                                                                                 //
// KGGZConnect: Display a connection dialog which allows in-place server addition. //
//                                                                                 //
/////////////////////////////////////////////////////////////////////////////////////

// Header file
#include "KGGZConnect.h"

// KGGZ includes
#include "KGGZCommon.h"
#include "KGGZLineSeparator.h"
#include "KGGZCaption.h"

// KDE classes
#include <kmessagebox.h>
#include <klocale.h>
#include <kapplication.h>
#include <kurl.h>

// Qt classes
#include <qlayout.h>
#include <qlabel.h>
#include <qradiobutton.h>
#include <qvalidator.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qpushbutton.h>
#include <qcheckbox.h>
#include <qbuttongroup.h>
#include <qsocket.h>
#include <qeventloop.h>
#include <qdir.h>

// System includes
#include <stdlib.h>

#ifdef WITH_HOWL
#include <sys/types.h>
#include <howl.h>
//#else
#endif
// FIXME: always included even if not used (due to moc's #ifndef handling)
#include <dnssd/query.h>

// GGZCore++ includes
#include "GGZCoreConfio.h"

#ifdef WITH_HOWL
static KGGZConnect *connectobj = NULL;
#endif
static QString connectstr;

/* Constructor: set up a small dialog for connections; provide server profile list */
KGGZConnect::KGGZConnect(QWidget *parent, const char *name)
: QWidget(parent, name, WType_Dialog)
{
	QVBoxLayout *vbox1, *vbox2;
	QHBoxLayout *hbox1, *hbox2, *hbox3, *hbox4;
	QLabel *label_host, *label_port, *label_name, *label_password;
	QRadioButton *radio_normal, *radio_guest, *radio_firsttime;
	QLabel *label_mode;
	QIntValidator *valid;
	KGGZLineSeparator *sep1, *sep2;
	KGGZCaption *caption;
	QFrame *frame;
	QHBoxLayout *hbox_frame;

	m_input = NULL;
	m_meta = NULL;
	m_sock = NULL;

	profile_select = new QComboBox(this);
	profile_new = new QPushButton(i18n("Create new profile"), this);
	profile_delete = new QPushButton(i18n("Delete profile"), this);
	profile_edit = new QPushButton(i18n("<< Edit"), this);

	m_pane = new QWidget(this);

	caption = new KGGZCaption(i18n("Connection"),
		i18n("Select your favorite connection profile\nand connect to a game server."),
		this);

	label_host = new QLabel(i18n("Server host:"), m_pane);
	label_port = new QLabel(i18n("Server port:"), m_pane);
	label_name = new QLabel(i18n("Log in as user/nickname:"), m_pane);
	label_password = new QLabel(i18n("Use password:"), m_pane);

	valid = new QIntValidator(this);
	valid->setRange(1024, 32767);

	frame = new QFrame(m_pane);
	frame->setFrameStyle(QFrame::Panel | QFrame::Sunken);

	button_select = new QPushButton(i18n("Select"), frame);

	input_host = new QLineEdit(frame);
	input_port = new QLineEdit(m_pane);
	input_port->setValidator(valid);
	input_name = new QLineEdit(m_pane);
	input_password = new QLineEdit(m_pane);

#ifdef KGGZ_WALLET
	input_password->setEnabled(false);
#else
	input_password->setEchoMode(QLineEdit::Password);
#endif

	button_ok = new QPushButton(i18n("Connect"), this);
	button_cancel = new QPushButton(i18n("Cancel"), this);

	label_mode = new QLabel(i18n("Log in as:"), m_pane);
	group_mode = new QButtonGroup(m_pane);
	group_mode->hide();
	radio_normal = new QRadioButton(i18n("normal"), m_pane);
	radio_guest = new QRadioButton(i18n("guest"), m_pane);
	radio_firsttime = new QRadioButton(i18n("starter"), m_pane);
	group_mode->insert(radio_normal, mode_normal);
	group_mode->insert(radio_guest, mode_guest);
	group_mode->insert(radio_firsttime, mode_firsttime);
	radio_normal->setChecked(true);

	option_server = new QCheckBox(i18n("Start ggzd server locally"), this);

#ifdef KGGZ_PATCH_ENCRYPTION
	option_tls = new QCheckBox(i18n("Use secure connection (TLS)"), this);
#endif

	sep1 = new KGGZLineSeparator(this);
	sep2 = new KGGZLineSeparator(m_pane);

	hbox_frame = new QHBoxLayout(frame, 2);
	hbox_frame->add(input_host);
	hbox_frame->add(button_select);

	vbox1 = new QVBoxLayout(this, 5);
	vbox1->add(caption);

	hbox2 = new QHBoxLayout(vbox1, 5);
	hbox2->add(profile_select);

	hbox4 = new QHBoxLayout(vbox1, 5);
	hbox4->add(profile_new);
	hbox4->add(profile_delete);
	hbox4->add(profile_edit);

	vbox1->add(sep1);

	vbox2 = new QVBoxLayout(m_pane, 5);
	vbox2->add(label_host);
	vbox2->add(frame);
	vbox2->add(label_port);
	vbox2->add(input_port);
	vbox2->add(label_name);
	vbox2->add(input_name);
	vbox2->add(label_password);
	vbox2->add(input_password);

	hbox3 = new QHBoxLayout(vbox2, 5);
	hbox3->add(label_mode);
	hbox3->add(radio_normal);
	hbox3->add(radio_guest);
	hbox3->add(radio_firsttime);

	vbox2->add(sep2);

	vbox1->add(m_pane);

	vbox1->add(option_server);
#ifdef KGGZ_PATCH_ENCRYPTION
	vbox1->add(option_tls);
#endif

	hbox1 = new QHBoxLayout(vbox1, 5);
	hbox1->add(button_ok);
	hbox1->add(button_cancel);

	connect(button_cancel, SIGNAL(clicked()), SLOT(close()));
	connect(button_ok, SIGNAL(clicked()), SLOT(slotAccept()));
	connect(group_mode, SIGNAL(clicked(int)), SLOT(slotModes(int)));
	connect(option_server, SIGNAL(clicked()), SLOT(slotInvoke()));
	connect(profile_select, SIGNAL(activated(int)), SLOT(slotLoadProfile(int)));
	connect(profile_new, SIGNAL(clicked()), SLOT(slotProfileNew()));
	connect(profile_delete, SIGNAL(clicked()), SLOT(slotProfileDelete()));
	connect(profile_edit, SIGNAL(clicked()), SLOT(slotPane()));
	connect(button_select, SIGNAL(clicked()), SLOT(slotProfileMeta()));

	m_connected = -1;
	m_loginmode = mode_normal;
	m_nosafe = 0;

	setCaption(i18n("Connect to server"));

	slotModes(m_loginmode);
	slotLoadProfile(-1);
}

/* Destructor */
KGGZConnect::~KGGZConnect()
{
}

void KGGZConnect::slotSaveProfile()
{
	GGZCoreConfio *config;
	QCString current;

	config = new GGZCoreConfio(QString("%1/.ggz/kggz.rc").arg(QDir::homeDirPath()).utf8(),
		GGZCoreConfio::readwrite | GGZCoreConfio::create);

	// Save previous profile and make new one the default (although it's empty)
	current = m_current.utf8();
	config->write(current, "Host", input_host->text());
	config->write(current, "Port", input_port->text());
	config->write(current, "Login", input_name->text());
#ifndef KGGZ_WALLET
	config->write(current, "Password", input_password->text());
#endif
	config->write(current, "Type", m_loginmode);
#ifdef KGGZ_PATCH_ENCRYPTION
	config->write(current, "Encryption", option_tls->isChecked());
#endif
	config->write("Session", "Defaultserver", profile_select->currentText());
	config->commit();
	delete config;
}

/* Loads a specified server profile, or the first if profile is -1 */
void KGGZConnect::slotLoadProfile(int profile)
{
	GGZCoreConfio *config;
	char **list = NULL;
	int i;
	int type;
	const char *listentry = NULL;
	bool usetls;
	QString server, defaultserver;
	QString host, port, username, password;

	if((profile != -1) && (!m_nosafe))
		slotSaveProfile();
	m_nosafe = 0;

	config = new GGZCoreConfio(QString("%1/.ggz/kggz.rc").arg(QDir::homeDirPath()).utf8(),
		GGZCoreConfio::readonly);

	// setup entries; try to find suitable first entry
	if(profile == -1)
	{
		profile_select->clear();
		listentry = config->read("Session", "Defaultserver", (char*)NULL);
		defaultserver = listentry;
		if(!defaultserver.isNull())
		{
			if((defaultserver != i18n("GGZ Meta Server"))
			&& ((defaultserver != i18n("LAN Game"))))
			{
				profile_select->insertItem(QPixmap(KGGZ_DIRECTORY "/images/icons/server.png"), defaultserver);
				profile_select->setCurrentItem(0);
			}
		}
		profile_select->insertItem(QPixmap(KGGZ_DIRECTORY "/images/icons/metaserver.png"), i18n("GGZ Meta Server"));
		if(defaultserver == i18n("GGZ Meta Server"))
			profile_select->setCurrentItem(0);
//#ifdef WITH_HOWL
		profile_select->insertItem(QPixmap(KGGZ_DIRECTORY "/images/icons/langame.png"), i18n("LAN Game"));
		if(defaultserver == i18n("LAN Game"))
			profile_select->setCurrentItem(1);
//#endif
		if(defaultserver.isNull())
		{
			modifyServerList(i18n("Default Stable Server"), 1);
		}
		
		config->read("Servers", "Servers", &i, &list);
		for(int j = 0; j < i; j++)
		{
			server = list[j];
			if(server != defaultserver)
				profile_select->insertItem(QPixmap(KGGZ_DIRECTORY "/images/icons/server.png"), server);
			GGZCoreConfio::free(list[j]);
		}
		if(list) GGZCoreConfio::free(list);
	}
	else listentry = profile_select->text(profile);

	// read values for selected server
	if(profile_select->currentText() == i18n("GGZ Meta Server"))
	{
		host = i18n("Automatic");
		port = "5688";
		input_host->setEnabled(false);
		input_port->setEnabled(false);
		button_select->setEnabled(false);
		profile_delete->setEnabled(false);
	}
	else if(profile_select->currentText() == i18n("LAN Game"))
	{
		host = i18n("Autodetect");
		port = "5688";
		input_host->setEnabled(false);
		input_port->setEnabled(false);
		button_select->setEnabled(false);
		profile_delete->setEnabled(false);
	}
	else
	{
		host = config->read(listentry, "Host", "live.ggzgamingzone.org");
		port = config->read(listentry, "Port", "5688");
		input_host->setEnabled(true);
		input_port->setEnabled(true);
		button_select->setEnabled(true);
		profile_delete->setEnabled(true);
	}
	username = config->read(listentry, "Login", i18n("GGZocker"));
	password = config->read(listentry, "Password", "");
	type = config->read(listentry, "Type", 1);
	usetls = config->read(listentry, "Encryption", false);

	// put values into fields
	input_host->setText(host);
	input_port->setText(port);
	input_name->setText(username);
#ifndef KGGZ_WALLET
	input_password->setText(password);
#endif
#ifdef KGGZ_PATCH_ENCRYPTION
	option_tls->setChecked(usetls);
#endif
	slotModes(type);

	// get rid of the configuration object
	delete config;

	m_current = profile_select->currentText();

	// Make sure even default entries are written
	slotSaveProfile();
}

/* Start a connection */
void KGGZConnect::slotAccept()
{
	slotSaveProfile();

	button_ok->setEnabled(false);

	if(input_host->text() == i18n("Automatic"))
	{
		m_sock = new QSocket();
		connect(m_sock, SIGNAL(connected()), SLOT(slotWrite()));
		connect(m_sock, SIGNAL(readyRead()), SLOT(slotRead()));
		m_sock->connectToHost("live.ggzgamingzone.org", 15689);
	}
	else if(input_host->text() == i18n("Autodetect"))
	{
		zeroconfQuery();
	}
	else
	{
		close();
		emit signalConnect(input_host->text(), input_port->text().toInt(),
			input_name->text(), input_password->text(), m_loginmode);
	}
}

void KGGZConnect::slotModes(int loginmode)
{
	m_loginmode = loginmode;
	group_mode->setButton(loginmode);

	if(loginmode == mode_normal)
	{
#ifdef KGGZ_WALLET
		input_password->setText(i18n("Using KWallet"));
#else
		input_password->setEnabled(true);
#endif
	}
	else
	{
#ifdef KGGZ_WALLET
		input_password->setText("");
#else
		input_password->setEnabled(false);
#endif
	}
}

void KGGZConnect::slotInvoke()
{
	if(option_server->isChecked())
		input_host->setEnabled(false);
	else
		input_host->setEnabled(true);
}

void KGGZConnect::slotProfileNew()
{
	// destroy input window to avoid duplicate signals
	if(m_input)
	{
		delete m_input;
		m_input = NULL;
	}

	// show input box and connect signal to slot
	if(!m_input) m_input = new KGGZInput(NULL, "KGGZInput",
		i18n("Profile identifier"), i18n("Chose a name for the new profile."));

	m_input->show();
	connect(m_input, SIGNAL(signalText(QString)), SLOT(slotProfileProcess(QString)));
}

void KGGZConnect::slotProfileMeta()
{
	if(!m_meta)
	{
		m_meta = new KGGZMeta(NULL, "KGGZMeta");
		connect(m_meta, SIGNAL(signalData(QString, QString)), SLOT(slotProfileMetaProcess(QString, QString)));
	}
	m_meta->show();
}

void KGGZConnect::slotProfileMetaProcess(QString host, QString port)
{
	input_host->setText(host);
	input_port->setText(port);
	m_meta->close();
}

void KGGZConnect::slotProfileDelete()
{
	modifyServerList(m_current, 2);
	profile_select->removeItem(profile_select->currentItem());
	m_nosafe = 1;
	slotLoadProfile(profile_select->currentItem());
}

void KGGZConnect::slotProfileProcess(QString identifier)
{
	if(identifier.isEmpty()) return;

	// Save current profile
	slotSaveProfile();

	// Add new profile
	profile_select->insertItem(QPixmap(KGGZ_DIRECTORY "/images/icons/server.png"), identifier, 0);
	profile_select->setCurrentItem(0);
	modifyServerList(identifier, 1);
	m_current = identifier;
	input_host->setText("");
	input_name->setText("");
	input_port->setText("5688");
	input_password->setText("");

	input_host->setEnabled(true);
	input_port->setEnabled(true);
	button_select->setEnabled(true);
	profile_delete->setEnabled(true);

	// Save it (although is is empty)
	slotSaveProfile();
}

// mode: 1 = add, 2 = delete
void KGGZConnect::modifyServerList(QString server, int mode)
{
	GGZCoreConfio *config;
	char **list = NULL;
	int i;
	QStringList servers;
	QString oldserver;

	server = server.replace(" ", "\\ ");

	config = new GGZCoreConfio(QString("%1/.ggz/kggz.rc").arg(QDir::homeDirPath()).utf8(),
		GGZCoreConfio::readwrite | GGZCoreConfio::create);

	// Update the list of available servers
	config->read("Servers", "Servers", &i, &list);

	for(int j = 0; j < i; j++)
	{
		oldserver = QString(list[j]).replace(" ", "\\ ");
		if((server != oldserver) || (mode == 1))
			servers << oldserver;
	}
	if(mode == 1) servers << server;

	config->write("Servers", "Servers", servers.join(" ").utf8());
	if(mode == 2)
	{
		config->removeSection(server.utf8());
		if(i < 2) config->removeKey("Servers", "Servers");
	}

	config->commit();
	delete config;

	for(int j = 0; j < i; j++)
		GGZCoreConfio::free(list[j]);
	if(list) GGZCoreConfio::free(list);
}

void KGGZConnect::slotPane()
{
	if(m_pane->isVisible())
	{
		m_pane->hide();
		profile_edit->setText(i18n("Edit >>"));
		setFixedHeight(100);
	}
	else
	{
		m_pane->show();
		profile_edit->setText(i18n("<< Edit"));
		setFixedHeight(minimumSizeHint().height());
	}
}

int KGGZConnect::optionServer()
{
	return option_server->isChecked();
}

int KGGZConnect::optionSecure()
{
#ifdef KGGZ_PATCH_ENCRYPTION
	return option_tls->isChecked();
#else
	return 0;
#endif
}

void KGGZConnect::slotRead()
{
	QString rdata;
	QString host, port;
	QStringList list;

	rdata = m_sock->readLine();
	rdata.truncate(rdata.length() - 1);

	if(!rdata.isEmpty())
	{
		list = list.split(':', rdata);
		if(list.count() == 3)
		{
			host = *(list.at(1));
			host = host.right(host.length() - 2);
			port = *(list.at(2));
			close();
			emit signalConnect(host, port.toInt(), input_name->text(), input_password->text(), m_loginmode);
		}
	}
}

void KGGZConnect::slotWrite()
{
	QString s;

	s = QString("query://ggz/connection/%1\n").arg(KGGZVERSION);
	m_sock->writeBlock(s.utf8(), s.length());
	m_sock->flush();
}

void KGGZConnect::showEvent(QShowEvent *e)
{
	Q_UNUSED(e);

	button_ok->setEnabled(true);
}

#ifdef WITH_HOWL
static sw_result reply(sw_discovery session, sw_discovery_oid oid,
	sw_uint32 interface,
	sw_const_string name, sw_const_string type, sw_const_string domain,
	sw_ipv4_address address, sw_port port, sw_octets tx, sw_ulong txl,
	sw_opaque extra)
{
	sw_string str;

	str = (char*)malloc(100);
	str = sw_ipv4_address_name(address, str, 100);

	connectstr = QString("ggz://%1:%2").arg(str).arg(port);
	return SW_OKAY;
}

static sw_result breply(sw_discovery session, sw_discovery_oid oid,
	sw_discovery_browse_status status, sw_uint32 interface,
	sw_const_string name, sw_const_string type, sw_const_string domain,
	sw_opaque extra)
{
	int ret;

	if(status == SW_DISCOVERY_BROWSE_ADD_SERVICE)
	{
		ret = sw_discovery_resolve(session, interface, name, type, domain, reply, NULL, &oid);
		if(ret)
		{
			KMessageBox::error(connectobj, i18n("Resolving failed."), i18n("Zeroconf error"));
			//connectobj->button_ok->setEnabled(true);
			connectobj->showEvent(NULL);
			connectstr = "";
			return -1;
		}
	}

	return SW_OKAY;
}
#endif

void KGGZConnect::slotService(DNSSD::RemoteService::Ptr ptr)
{
#ifndef WITH_HOWL
	ptr->resolve();
	connectstr = QString("ggz://%1:%2").arg(ptr->hostName()).arg(ptr->port());
#endif
}

void KGGZConnect::slotServiceFinished()
{
#ifndef WITH_HOWL
	KMessageBox::error(this, i18n("Resolving failed."), i18n("Zeroconf error"));
	button_ok->setEnabled(true);
	connectstr = "";
#endif
}

void KGGZConnect::zeroconfQuery()
{
	connectstr = QString::null;

#ifdef WITH_HOWL
	int ret;
	sw_discovery session;
	sw_discovery_oid oid;

	connectobj = this;

	ret = sw_discovery_init(&session);
	if(ret != SW_OKAY)
	{
		KMessageBox::error(this, i18n("Zeroconf could not be initialized."), i18n("Zeroconf error"));
		return;
	}

	ret = sw_discovery_browse(session, 0, "_ggz._tcp", NULL, breply, NULL, &oid);

	while(connectstr.isNull())
	{
		sw_discovery_read_socket(session);
		kapp->eventLoop()->processEvents(QEventLoop::AllEvents);
	}
#else
	DNSSD::Query *query = new DNSSD::Query("_ggz._tcp", QString::null/*"local."*/);
	connect(query,
		SIGNAL(serviceAdded(DNSSD::RemoteService::Ptr)),
		SLOT(slotService(DNSSD::RemoteService::Ptr)));
	connect(query, SIGNAL(finished()), SLOT(slotServiceFinished()));
	query->startQuery();

	while(connectstr.isNull())
	{
		kapp->eventLoop()->processEvents(QEventLoop::AllEvents);
	}

	delete query;
#endif

	if(connectstr.isEmpty()) return;

	KURL url(connectstr);

	close();
	KMessageBox::information(this,
		i18n("A GGZ server was found on %1.").arg(connectstr),
		i18n("Zeroconf success"));
	emit signalConnect(url.host(), url.port(), input_name->text(),
		input_password->text(), m_loginmode);
}

