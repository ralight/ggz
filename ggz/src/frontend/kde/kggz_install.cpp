#include "kggz_install.h"

#include <qpushbutton.h>
#include <qlayout.h>
#include <klocale.h>

KGGZ_Install::KGGZ_Install(QWidget *parent, char *name)
: QWidget(parent, name)
{
	QLayout *vbox;
	QPushButton *ok;

	m_output = new QMultiLineEdit(this);
	ok = new QPushButton("OK", this);

	vbox = new QVBoxLayout(this, 5);
	vbox->add(m_output);
	vbox->add(ok);

	connect(ok, SIGNAL(clicked()), SLOT(slotClose()));

	m_output->append(i18n("Configuring game modules..."));

	setFixedSize(200, 300);
	setCaption("Module Installation");
}

KGGZ_Install::~KGGZ_Install()
{
}

void KGGZ_Install::slotClose()
{
	close();
}
