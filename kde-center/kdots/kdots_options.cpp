/////////////////////////////////////////////////////////////////////
//
// KDots: Connect the Dots game for KDE 4
// http://www.ggzgamingzone.org/gameclients/kdots/
//
// Copyright (C) 2001 - 2008 Josef Spillner <josef@ggzgamingzone.org>
// Published under the conditions of the GNU GPL, see COPYING
//
/////////////////////////////////////////////////////////////////////

#include "kdots_options.h"

#include <klocale.h>

#include <QPushButton>
#include <QLayout>
#include <QLabel>

KDotsOptions::KDotsOptions()
: QWidget()
{
	QPushButton *ok;
	QVBoxLayout *vbox, *vbox1, *vbox2, *vbox3;
	QHBoxLayout *hbox;
	QLabel *labelh, *labelv;
	QLabel *title, *desc;

	title = new QLabel(i18n("Options"));
	//QFont font("helvetica", 11);
	QFont font = title->font();
	font.setBold(TRUE);
	title->setFont(font);

	desc = new QLabel(i18n("Please specify the board size here."));

	labelh = new QLabel(i18n("Horizontal"));
	labelv = new QLabel(i18n("Vertical"));

	sliderh = new QSlider(Qt::Horizontal);
	sliderh->setRange(2, 24);
	sliderh->setValue(7);
	sliderh->setTickPosition(QSlider::TicksBelow);
	sliderh->setTickInterval(1);
	sliderh->setPageStep(1);

	sliderv = new QSlider(Qt::Horizontal);
	sliderv->setRange(2, 24);
	sliderv->setValue(7);
	sliderv->setTickPosition(QSlider::TicksBelow);
	sliderv->setTickInterval(1);
	sliderv->setPageStep(1);

	labelhn = new QLabel("7");
	labelvn = new QLabel("7");

	ok = new QPushButton("OK");

	vbox = new QVBoxLayout();
	setLayout(vbox);
	vbox->addWidget(title);
	vbox->addWidget(desc);
	hbox = new QHBoxLayout();
	vbox->addLayout(hbox);
	vbox1 = new QVBoxLayout();
	hbox->addLayout(vbox1);
	vbox2 = new QVBoxLayout();
	hbox->addLayout(vbox2);
	vbox3 = new QVBoxLayout();
	hbox->addLayout(vbox3);
	vbox1->addWidget(labelh);
	vbox1->addWidget(labelv);
	vbox2->addWidget(sliderh);
	vbox2->addWidget(sliderv);
	vbox3->addWidget(labelhn);
	vbox3->addWidget(labelvn);
	vbox->addWidget(ok);

	connect(ok, SIGNAL(clicked()), SLOT(slotAccepted()));
	connect(sliderh, SIGNAL(valueChanged(int)), SLOT(slotValueHorizontal(int)));
	connect(sliderv, SIGNAL(valueChanged(int)), SLOT(slotValueVertical(int)));

	setWindowTitle(i18n("KDots Options"));
	//resize(300, 150);
	show();
}

KDotsOptions::~KDotsOptions()
{
}

void KDotsOptions::slotAccepted()
{
	emit signalAccepted(sliderh->value(), sliderv->value());
	close();
}

void KDotsOptions::slotValueHorizontal(int value)
{
	QString str;

	str.setNum(value);
	labelhn->setText(str);
}

void KDotsOptions::slotValueVertical(int value)
{
	QString str;

	str.setNum(value);
	labelvn->setText(str);
}
