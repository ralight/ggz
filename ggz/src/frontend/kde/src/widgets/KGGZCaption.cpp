#include "KGGZCaption.h"

#include <qpalette.h>
#include <qlabel.h>
#include <qlayout.h>

KGGZCaption::KGGZCaption(QString caption, QString explanation, QWidget *parent, const char *name)
: QFrame(parent, name)
{
	QLabel *label, *label2;
	QPalette pal;
	QVBoxLayout *vbox;

	label = new QLabel(caption, this);
	label2 = new QLabel(explanation, this);

	QFont font("helvetica", 11);
	font.setBold(TRUE);
	label->setFont(font, QFont::Black);

	setBackgroundColor(QColor(100, 0, 0));
	label->setBackgroundColor(QColor(100, 0, 0));
	label2->setBackgroundColor(QColor(100, 0, 0));

	pal = palette();
	pal.setColor(QColorGroup::Foreground, QColor(255, 255, 255));
	label->setPalette(pal);
	label2->setPalette(pal);

	vbox = new QVBoxLayout(this, 5);
	vbox->add(label);
	vbox->add(label2);

	setFrameStyle(Panel | Sunken);
	setFixedHeight(60);
	show();
}

KGGZCaption::~KGGZCaption()
{
}

