///////////////////////////////////////////////////////////////
//
// KDots
// Connect the Dots game for KDE, using the Dots classes
// Copyright (C) 2001, 2002 Josef Spillner
// dr_maux@users.sourceforge.net
// The MindX Open Source Project
// http://mindx.sourceforge.net/games/kdots/
//
///////////////////////////////////////////////////////////////

#include "kdots_options.h"

#include <klocale.h>

#include <qpushbutton.h>
#include <qlayout.h>

KDotsOptions::KDotsOptions(QWidget *parent, const char *name)
: QWidget(parent, name)
{
	QPushButton *ok;
	QVBoxLayout *vbox, *vbox1, *vbox2, *vbox3;
	QHBoxLayout *hbox;
	QLabel *labelh, *labelv;
	QLabel *title, *desc;

	title = new QLabel(i18n("Options"), this);
	//QFont font("helvetica", 11);
	QFont font = title->font();
	font.setBold(TRUE);
	title->setFont(font);

	desc = new QLabel(i18n("Please specify the board size here."), this);

	labelh = new QLabel(i18n("Horizontal"), this);
	labelv = new QLabel(i18n("Vertical"), this);

	sliderh = new QSlider(QSlider::Horizontal, this);
	sliderh->setRange(2, 24);
	sliderh->setValue(7);
	sliderh->setTickmarks(QSlider::Below);
	sliderh->setTickInterval(1);
	sliderh->setPageStep(1);

	sliderv = new QSlider(QSlider::Horizontal, this);
	sliderv->setRange(2, 24);
	sliderv->setValue(7);
	sliderv->setTickmarks(QSlider::Below);
	sliderv->setTickInterval(1);
	sliderv->setPageStep(1);

	labelhn = new QLabel("7", this);
	labelvn = new QLabel("7", this);

	ok = new QPushButton("OK", this);

	vbox = new QVBoxLayout(this, 5);
	vbox->add(title);
	vbox->add(desc);
	hbox = new QHBoxLayout(vbox, 5);
	vbox1 = new QVBoxLayout(hbox, 5);
	vbox2 = new QVBoxLayout(hbox, 5);
	vbox3 = new QVBoxLayout(hbox, 5);
	vbox1->add(labelh);
	vbox1->add(labelv);
	vbox2->add(sliderh);
	vbox2->add(sliderv);
	vbox3->add(labelhn);
	vbox3->add(labelvn);
	vbox->add(ok);

	connect(ok, SIGNAL(clicked()), SLOT(slotAccepted()));
	connect(sliderh, SIGNAL(valueChanged(int)), SLOT(slotValueHorizontal(int)));
	connect(sliderv, SIGNAL(valueChanged(int)), SLOT(slotValueVertical(int)));

	setCaption(i18n("KDots Options"));
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
