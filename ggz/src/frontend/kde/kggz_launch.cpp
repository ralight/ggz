#include "kggz_launch.h"
#include "kggz_teams.h"
#include <qlayout.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qslider.h>
#include <klocale.h>

KGGZ_Launch::KGGZ_Launch(QWidget *parent, char *name)
: QWidget(parent, name)
{
	QVBoxLayout *vbox1;
	QHBoxLayout *hbox1, *hbox2, *hbox3, *hbox4, *hbox5, *hbox6;

	QPushButton *button_ok, *button_cancel;
	QLabel *label_description, *label_type;
	QLineEdit *input_description, *input_type;
	QLabel *label_open, *label_reserved, *label_ai;
	QSlider *slider_open, *slider_reserved, *slider_ai;

	label_type = new QLabel(i18n("Game type:"), this);
	label_description = new QLabel(i18n("Description:"), this);

	input_type = new QLineEdit(this);
	input_description = new QLineEdit(this);

	label_open = new QLabel(i18n("Open seats:"), this);
	label_reserved = new QLabel(i18n("Reserved:"), this);
	label_ai = new QLabel(i18n("AI players:"), this);

	count_open = new QLabel("0", this);
	count_reserved = new QLabel("0", this);
	count_ai = new QLabel("0", this);

	slider_open = new QSlider(this);
	slider_reserved = new QSlider(this);
	slider_ai = new QSlider(this);
	slider_open->setOrientation(QSlider::Horizontal);
	slider_reserved->setOrientation(QSlider::Horizontal);
	slider_ai->setOrientation(QSlider::Horizontal);

	button_ok = new QPushButton("OK", this);
	button_cancel = new QPushButton(i18n("Cancel"), this);

	vbox1 = new QVBoxLayout(this, 5);

	hbox2 = new QHBoxLayout(vbox1, 5);
	hbox2->add(label_type);
	hbox2->add(input_type);

	hbox3 = new QHBoxLayout(vbox1, 5);
	hbox3->add(label_description);
	hbox3->add(input_description);

	hbox4 = new QHBoxLayout(vbox1, 5);
	hbox4->add(label_open);
	hbox4->add(slider_open);
	hbox4->add(count_open);

	hbox5 = new QHBoxLayout(vbox1, 5);
	hbox5->add(label_reserved);
	hbox5->add(slider_reserved);
	hbox5->add(count_reserved);

	hbox6 = new QHBoxLayout(vbox1, 5);
	hbox6->add(label_ai);
	hbox6->add(slider_ai);
	hbox6->add(count_ai);

	hbox1 = new QHBoxLayout(vbox1, 5);
	hbox1->add(button_ok);
	hbox1->add(button_cancel);

	connect(button_cancel, SIGNAL(clicked()), SLOT(close()));
	connect(button_ok, SIGNAL(clicked()), SLOT(accept()));

	connect(slider_open, SIGNAL(valueChanged(int)), SLOT(setOpenSeats(int)));
	connect(slider_reserved, SIGNAL(valueChanged(int)), SLOT(setReservedSeats(int)));
	connect(slider_ai, SIGNAL(valueChanged(int)), SLOT(setAISeats(int)));

	setCaption(i18n("Launch new game"));
	resize(300, 200);
}

KGGZ_Launch::~KGGZ_Launch()
{
}

void KGGZ_Launch::accept()
{
	KGGZ_Teams *teams;

	teams = new KGGZ_Teams(NULL, "teams");
	close();
}

void KGGZ_Launch::setOpenSeats(int seats)
{
	count_open->setNum(seats);
}

void KGGZ_Launch::setReservedSeats(int seats)
{
	count_reserved->setNum(seats);
}

void KGGZ_Launch::setAISeats(int seats)
{
	count_ai->setNum(seats);
}
