/* Header */
#include "kggz_settings.h"

/* Qt files */
#include <qlayout.h>
#include <qpushbutton.h>

/* KDE files */
#include <klocale.h>

/* System files */
#include <stdlib.h>
#include <stdio.h>

/* Configuration functions: ggzcore_conf_... */
#include <ggzcore.h>

/* Section in the configuration file */
#define KGGZ_SETTINGS "KGGZ-Settings"

/* free() is dangerous for most of us */
#define free(x); if(x) free(x); else printf("FREE FAILED!\n");

KGGZ_Settings::KGGZ_Settings(QWidget *parent, char *name)
: QWidget(parent, name)
{
	QVBoxLayout *vbox1;
	QHBoxLayout *hbox1;
	QPushButton *button_ok, *button_cancel;

	option_chatlog = new QCheckBox(i18n("Chat logging"), this);
	option_session = new QCheckBox(i18n("Session management"), this);
	option_feature = new QCheckBox(i18n("Full-featured client"), this);
	option_startup = new QCheckBox(i18n("Show connection dialog on startup"), this);
	option_java = new QCheckBox(i18n("Enable Java in the browser"), this);

	button_ok = new QPushButton("OK", this);
	button_cancel = new QPushButton(i18n("Cancel"), this);

	vbox1 = new QVBoxLayout(this, 5);
	vbox1->add(option_chatlog);
	vbox1->add(option_session);
	vbox1->add(option_feature);
	vbox1->add(option_startup);
	vbox1->add(option_java);

	hbox1 = new QHBoxLayout(vbox1, 5);
	hbox1->add(button_ok);
	hbox1->add(button_cancel);

	connect(button_cancel, SIGNAL(clicked()), SLOT(slotClose()));
	connect(button_ok, SIGNAL(clicked()), SLOT(slotAccept()));

	setCaption(i18n("Settings"));
	resize(300, 200);

	readValues();
}

KGGZ_Settings::~KGGZ_Settings()
{
}

void KGGZ_Settings::slotClose()
{
	emit signalClosed();
	close();
}

void KGGZ_Settings::slotAccept()
{
	writeValues();
	slotClose();
}

void KGGZ_Settings::readValues()
{
	option_chatlog->setChecked(ggzcore_conf_read_int(KGGZ_SETTINGS, "Chatlog", 0));
	option_session->setChecked(ggzcore_conf_read_int(KGGZ_SETTINGS, "Session", 0));
	option_feature->setChecked(ggzcore_conf_read_int(KGGZ_SETTINGS, "Feature", 1));
	option_startup->setChecked(ggzcore_conf_read_int(KGGZ_SETTINGS, "Startup", 0));
	option_java->setChecked(ggzcore_conf_read_int(KGGZ_SETTINGS, "Java", 0));
}

void KGGZ_Settings::writeValues()
{
	ggzcore_conf_write_int(KGGZ_SETTINGS, "Chatlog", option_chatlog->isChecked());
	ggzcore_conf_write_int(KGGZ_SETTINGS, "Session", option_session->isChecked());
	ggzcore_conf_write_int(KGGZ_SETTINGS, "Feature", option_feature->isChecked());
	ggzcore_conf_write_int(KGGZ_SETTINGS, "Startup", option_startup->isChecked());
	ggzcore_conf_write_int(KGGZ_SETTINGS, "Java", option_java->isChecked());

	ggzcore_conf_commit();
}
