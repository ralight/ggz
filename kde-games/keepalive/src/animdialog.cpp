#include "animdialog.h"

#include <qcanvas.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qpushbutton.h>

#include <klocale.h>

AnimDialog::AnimDialog(QWidget *parent, const char *name)
: QWidget(parent, name)
{
	QHBoxLayout *hbox;
	QVBoxLayout *vbox;
	QPushButton *ok;

	m_a = NULL;
	m_counter = 0;

	m_f = new QFrame(this);
	m_f->setFixedSize(72, 72);
	m_f->setFrameStyle(QFrame::Sunken | QFrame::Panel);

	m_label = new QLabel("", this);

	ok = new QPushButton(i18n("Dismiss"), this);

	vbox = new QVBoxLayout(this, 5);
	hbox = new QHBoxLayout(vbox, 5);
	hbox->add(m_f);
	hbox->add(m_label);
	vbox->add(ok);

	resize(400, 100);
	setCaption(i18n("Information"));

	connect(ok, SIGNAL(clicked()), SLOT(close()));

	startTimer(500);
}

AnimDialog::~AnimDialog()
{
}

void AnimDialog::setAnimation(QCanvasPixmapArray *a)
{
	m_a = a;
	timerEvent(NULL);
}

void AnimDialog::setText(QString text)
{
	m_label->setText(text);
}

void AnimDialog::timerEvent(QTimerEvent *e)
{
	Q_UNUSED(e);

	if(m_a)
	{
		m_counter = (m_counter + 1) % m_a->count();
		m_f->setBackgroundPixmap(*(m_a->image(m_counter)));
	}
}


