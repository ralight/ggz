/* Header */
#include "kggz_playerinfo.h"

/* Qt files */
#include <qlayout.h>
#include <qpushbutton.h>
#include <qlabel.h>

/* KDE files */
#include <klocale.h>

/* System files */
#include <stdlib.h>
#include <stdio.h>

/* Configuration functions: ggzcore_conf_... */
#include <ggzcore.h>

/* Section in the configuration file */
#define KGGZ_SETTINGS "KGGZ-Playerinfo"

/* free() is dangerous for most of us */
#define free(x); if(x) free(x); else printf("FREE FAILED!\n");

KGGZ_Playerinfo::KGGZ_Playerinfo(QWidget *parent, char *name)
: QWidget(parent, name)
{
	QVBoxLayout *vbox1;
	QHBoxLayout *hbox1, *hbox2, *hbox3, *hbox4, *hbox5, *hbox6;
	QPushButton *button_ok, *button_cancel;
	QLabel *label_name, *label_country, *label_email, *label_homepage, *label_city;

	label_name = new QLabel(i18n("Your Name:"), this);
	label_country = new QLabel(i18n("Your Country or State:"), this);
	label_email = new QLabel(i18n("Email address"), this);
	label_homepage = new QLabel(i18n("Homepage"), this);
	label_city = new QLabel(i18n("City"), this);

	input_name = new QLineEdit(this);
	input_country = new QLineEdit(this);
	input_email = new QLineEdit(this);
	input_homepage = new QLineEdit(this);
	input_city = new QLineEdit(this);

	option_holdback = new QCheckBox(i18n("Hold back all information"), this);

	button_ok = new QPushButton("OK", this);
	button_cancel = new QPushButton(i18n("Cancel"), this);

	vbox1 = new QVBoxLayout(this, 5);

	hbox1 = new QHBoxLayout(vbox1, 5);
	hbox1->add(label_name);
	hbox1->add(input_name);

	hbox2 = new QHBoxLayout(vbox1, 5);
	hbox2->add(label_country);
	hbox2->add(input_country);

	hbox3 = new QHBoxLayout(vbox1, 5);
	hbox3->add(label_email);
	hbox3->add(input_email);

	hbox4 = new QHBoxLayout(vbox1, 5);
	hbox4->add(label_homepage);
	hbox4->add(input_homepage);

	hbox5 = new QHBoxLayout(vbox1, 5);
	hbox5->add(label_city);
	hbox5->add(input_city);

	vbox1->add(option_holdback);

	hbox6 = new QHBoxLayout(vbox1, 5);
	hbox6->add(button_ok);
	hbox6->add(button_cancel);

	connect(button_cancel, SIGNAL(clicked()), SLOT(slotClose()));
	connect(button_ok, SIGNAL(clicked()), SLOT(slotAccept()));

	setCaption(i18n("Player information"));
	resize(300, 200);

	readValues();
}

KGGZ_Playerinfo::~KGGZ_Playerinfo()
{
}

void KGGZ_Playerinfo::slotClose()
{
	emit signalClosed();
	close();
}

void KGGZ_Playerinfo::slotAccept()
{
	writeValues();
	slotClose();
}

void KGGZ_Playerinfo::readValues()
{
	input_name->setText(ggzcore_conf_read_string(KGGZ_SETTINGS, "Name", ""));
	input_country->setText(ggzcore_conf_read_string(KGGZ_SETTINGS, "Country", ""));
	input_email->setText(ggzcore_conf_read_string(KGGZ_SETTINGS, "Email", ""));
	input_homepage->setText(ggzcore_conf_read_string(KGGZ_SETTINGS, "Homepage", ""));
	input_city->setText(ggzcore_conf_read_string(KGGZ_SETTINGS, "City", ""));
	option_holdback->setChecked(ggzcore_conf_read_int(KGGZ_SETTINGS, "Holdback", 0));
}

void KGGZ_Playerinfo::writeValues()
{
	char *save_name = NULL;
	char *save_country = NULL;
	char *save_email = NULL;
	char *save_homepage = NULL;
	char *save_city = NULL;

	save_name = (char*)malloc(strlen(input_name->text()) + 1);
	save_country = (char*)malloc(strlen(input_country->text()) + 1);
	save_email = (char*)malloc(strlen(input_email->text()) + 1);
	save_homepage = (char*)malloc(strlen(input_homepage->text()) + 1);
	save_city = (char*)malloc(strlen(input_city->text()) + 1);

	strcpy(save_name, input_name->text().latin1());
	strcpy(save_country, input_country->text().latin1());
	strcpy(save_email, input_email->text().latin1());
	strcpy(save_homepage, input_homepage->text().latin1());
	strcpy(save_city, input_city->text().latin1());

	ggzcore_conf_write_string(KGGZ_SETTINGS, "Name", save_name);
	ggzcore_conf_write_string(KGGZ_SETTINGS, "Country", save_country);
	ggzcore_conf_write_string(KGGZ_SETTINGS, "Email", save_email);
	ggzcore_conf_write_string(KGGZ_SETTINGS, "Homepage", save_homepage);
	ggzcore_conf_write_string(KGGZ_SETTINGS, "City", save_city);
	ggzcore_conf_write_int(KGGZ_SETTINGS, "Holdback", option_holdback->isChecked());

	free(save_name);
	free(save_country);
	free(save_email);
	free(save_homepage);
	free(save_city);

	ggzcore_conf_commit();
}
