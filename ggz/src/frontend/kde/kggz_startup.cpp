#include "kggz_startup.h"
#include <qlayout.h>
#include <qpixmap.h>

QWidget *logo;

KGGZ_Startup::KGGZ_Startup(QWidget *parent, char *name)
: QWidget(parent, name)
{
	QVBoxLayout *vbox;

	logo = new QWidget(this);
	logo->setFixedSize(500, 400);

	vbox = new QVBoxLayout(this, 0);
	vbox->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	vbox->add(logo);
}

KGGZ_Startup::~KGGZ_Startup()
{
}

void KGGZ_Startup::setBackgroundPixmap(QPixmap pixmap)
{
	logo->setBackgroundPixmap(pixmap);
}

