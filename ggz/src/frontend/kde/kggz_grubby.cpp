#include "kggz_grubby.h"

#include <qlayout.h>
#include <qframe.h>
#include <qpixmap.h>
#include <qlabel.h>
#include <klocale.h>
#include <qmultilineedit.h>
#include <qpushbutton.h>
#include "config.h"

KGGZ_Grubby::KGGZ_Grubby(QWidget *parent, char *name)
: QWidget(parent, name)
{
	QHBoxLayout *hbox;
	QVBoxLayout *vbox;
	QWidget *grubby;
	QFrame *main;
	QLabel *grubbytitle, *grubbytext;
	QMultiLineEdit *messages;
	QPushButton *done;

	grubby = new QWidget(this);
	grubby->setFixedSize(128, 128);
	grubby->setBackgroundPixmap(QPixmap(QString(instdir) + "images/grubbybig.png"));

	main = new QFrame(this);
	main->setFrameStyle(QFrame::Panel | QFrame::Sunken);

	grubbytitle = new QLabel("<font size+=1 color=#0000ff><b><i>Grubby</i></b></font>", this);
	grubbytext = new QLabel(i18n("Hi! I am Grubby,<br>"
		"and I am responsible<br>"
		"for private player<br>"
		"messages. May I help<br>"
		"you please?"), this);

	messages = new QMultiLineEdit(main);
	messages->setWordWrap(QMultiLineEdit::WidgetWidth);
	messages->setText(i18n("No messages scheduled for you."));
	messages->resize(150, 300);

	done = new QPushButton(i18n("Done"), this);

	hbox = new QHBoxLayout(this, 5);

	vbox = new QVBoxLayout(hbox, 5);
	vbox->add(grubby);
	vbox->add(grubbytitle);
	vbox->add(grubbytext);
	vbox->add(done);

	hbox->add(main);

	connect(done, SIGNAL(clicked()), SLOT(slotClose()));

	//resize(300, 300);
	setFixedSize(300, 300);
	setCaption("GGZ Grubby");
	show();
}

KGGZ_Grubby::~KGGZ_Grubby()
{
}

void KGGZ_Grubby::slotClose()
{
	close();
}

/* TODO for 0.0.5!!!
void KGGZ_Grubby::slotResizeEvent(QResizeEvent *e)
{
	resize(e->size());
}
*/