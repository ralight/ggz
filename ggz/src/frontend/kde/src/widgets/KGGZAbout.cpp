#include "KGGZAbout.h"

#include <qlayout.h>
#include <qpushbutton.h>
#include <qpainter.h>

KGGZAbout::KGGZAbout(QWidget *parent = NULL, char *name = NULL)
: QWidget(parent, name, WStyle_Customize | WStyle_Tool | WStyle_DialogBorder)
{
	QVBoxLayout *vbox;
	QPushButton *ok;

	m_frame = new QFrame(this);
	m_frame->setFrameStyle(QFrame::Panel | QFrame::Sunken);
	m_frame->setBackgroundColor(QColor(100.0, 0.0, 0.0));

	ok = new QPushButton("OK", this);

	vbox = new QVBoxLayout(this, 5);
	vbox->add(m_frame);
	vbox->add(ok);

	connect(ok, SIGNAL(clicked()), SLOT(close()));

	startTimer(70);

	setFixedSize(350, 250);
	setCaption("About the GGZ Gaming Zone");
	show();
}

KGGZAbout::~KGGZAbout()
{
}

void KGGZAbout::timerEvent(QTimerEvent *e)
{
	paintEvent(NULL);
}

void KGGZAbout::paintEvent(QPaintEvent *e)
{
	QPainter p;
	QFont font;
	static int x = 50;
	static int x2 = 100;

	font = QFont("helvetica", 18);
	font.setStyleHint(QFont::Serif);

	p.begin(m_frame);
	p.fillRect(0, 110, 350, 70, QBrush(QColor(100.0, 0.0, 0.0)));
	p.setFont(font);
	p.setPen(QColor(255.0, 255.0, 255.0));
	p.drawText(10, 25, "The GGZ Gaming Zone");
	p.drawLine(10, 30, 330, 30);
	p.drawLine(10, 32, 330, 32);
	font.setPointSize(10);
	p.setFont(font);
	p.drawText(10, 45, "Free Software shall dominate all Online Games!");
	p.drawText(10, 65, "The GGZ Gaming Zone is a collaboration between several people who");
	p.drawText(10, 77, "try to make public online game services available for free -");
	p.drawText(10, 89, "including source code and professional help for both gamers and");
	p.drawText(10, 101, "game developers. GGZ is available for many platforms and toolkits.");
	p.drawText(10, 185, "Homepage: http://ggz.sourceforge.net");
	font.setPointSize(50);
	font.setWeight(QFont::Black);
	p.setFont(font);
	p.setPen(QColor(130.0, 0.0, 0.0));
	p.drawText(x, 160, "GGZ Gaming Zone 0.0.4");
	font.setPointSize(10);
	font.setWeight(QFont::Normal);
	p.setFont(font);
	p.setPen(QColor(210.0, 50.0, 50.0));
	p.drawText(x2, 160, "Brian Cox - Brent Hendricks - Doug Hudson - Rich Gade - Ismael Orenstein - Dan Papasian - "
		"Ricardo Quesada - Josef Spillner - Justin Zaun - and many supporters...");
	p.end();
	x--;
	x2 -= 3;
	if(x < -530) x = 350;
	if(x2 < -770) x2 = 350;
}

