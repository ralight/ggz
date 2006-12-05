//////////////////////////////////////////////////////////////////////
// KConnectX
// Copyright (C) 2006 Josef Spillner <josef@ggzgamingzone.org>
// Published under GNU GPL conditions
//////////////////////////////////////////////////////////////////////

// KConnectX includes
#include "opt.h"

// KDE includes
#include <klocale.h>

// Qt includes
#include <qslider.h>
#include <qlayout.h>
#include <qlabel.h>

Opt::Opt(QWidget *parent, const char *name)
: KDialogBase(parent, name, true, QString::null, KDialogBase::Ok)
{
	QWidget *root = makeMainWidget();

	m_boardwidth = new QSlider(root);
	m_boardwidth->setOrientation(QSlider::Horizontal);

	m_boardheight = new QSlider(root);
	m_boardheight->setOrientation(QSlider::Horizontal);

	m_connectlength = new QSlider(root);
	m_connectlength->setOrientation(QSlider::Horizontal);

	m_lboardwidth = new QLabel(QString::null, root);
	m_lboardheight = new QLabel(QString::null, root);
	m_lconnectlength = new QLabel(QString::null, root);

	connect(m_boardwidth, SIGNAL(valueChanged(int)), SLOT(slotWidth(int)));
	connect(m_boardheight, SIGNAL(valueChanged(int)), SLOT(slotHeight(int)));
	connect(m_connectlength, SIGNAL(valueChanged(int)), SLOT(slotLength(int)));

	QVBoxLayout *vbox = new QVBoxLayout(root, 5);
	vbox->add(m_lboardwidth);
	vbox->add(m_boardwidth);
	vbox->add(m_lboardheight);
	vbox->add(m_boardheight);
	vbox->add(m_lconnectlength);
	vbox->add(m_connectlength);
}

void Opt::slotWidth(int width)
{
	m_lboardwidth->setText(i18n("Board width: %1").arg(width));
}

void Opt::slotHeight(int height)
{
	m_lboardheight->setText(i18n("Board height: %1").arg(height));
}

void Opt::slotLength(int length)
{
	m_lconnectlength->setText(i18n("Connection length: %1").arg(length));
}

void Opt::setBoardHeightRange(char min, char max)
{
	m_boardheight->setMinValue(min);
	m_boardheight->setMaxValue(max);

	if(min == max) m_boardheight->setEnabled(false);
	else m_boardheight->setEnabled(true);

	slotHeight(min);
}

void Opt::setBoardWidthRange(char min, char max)
{
	m_boardwidth->setMinValue(min);
	m_boardwidth->setMaxValue(max);

	if(min == max) m_boardwidth->setEnabled(false);
	else m_boardwidth->setEnabled(true);

	slotWidth(min);
}

void Opt::setConnectLengthRange(char min, char max)
{
	m_connectlength->setMinValue(min);
	m_connectlength->setMaxValue(max);

	if(min == max) m_connectlength->setEnabled(false);
	else m_connectlength->setEnabled(true);

	slotLength(min);
}

char Opt::boardHeight()
{
	return m_boardheight->value();
}

char Opt::boardWidth()
{
	return m_boardwidth->value();
}

char Opt::connectLength()
{
	return m_connectlength->value();
}

