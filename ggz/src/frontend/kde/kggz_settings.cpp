#include "kggz_settings.h"
#include <qlayout.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qlineedit.h>

KGGZ_Settings::KGGZ_Settings(QWidget *parent, char *name)
: QWidget(parent, name)
{
	QVBoxLayout *vbox1;
	QHBoxLayout *hbox1, *hbox2, *hbox3;
	QCheckBox *option_chatlog, *option_session, *option_feature;
	QPushButton *button_ok, *button_cancel;
	QLabel *label_name, *label_country;
	QLineEdit *input_name, *input_country;

	option_chatlog = new QCheckBox("Chat logging", this);
	option_session = new QCheckBox("Session management", this);
	option_feature = new QCheckBox("Full-featured client", this);

	label_name = new QLabel("Your Name:", this);
	label_country = new QLabel("Your Country or State:", this);

	input_name = new QLineEdit(this);
	input_country = new QLineEdit(this);

	button_ok = new QPushButton("OK", this);
	button_cancel = new QPushButton("Cancel", this);

	vbox1 = new QVBoxLayout(this, 5);
	vbox1->add(option_chatlog);
	vbox1->add(option_session);
	vbox1->add(option_feature);

	hbox2 = new QHBoxLayout(vbox1, 5);
	hbox2->add(label_name);
	hbox2->add(input_name);

	hbox3 = new QHBoxLayout(vbox1, 5);
	hbox3->add(label_country);
	hbox3->add(input_country);

	hbox1 = new QHBoxLayout(vbox1, 5);
	hbox1->add(button_ok);
	hbox1->add(button_cancel);

	connect(button_cancel, SIGNAL(clicked()), SLOT(close()));
	connect(button_ok, SIGNAL(clicked()), SLOT(accept()));

	setCaption("Settings");
	resize(300, 200);
}

KGGZ_Settings::~KGGZ_Settings()
{
}

void KGGZ_Settings::accept()
{
	close();
}