#include "kggztableview.h"

#include <klocale.h>
#include <kseparator.h>
#include <kcombobox.h>
#include <kstandarddirs.h>

#include <qlayout.h>
#include <qframe.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qlcdnumber.h>
#include <qscrollarea.h>
#include <qimage.h>
#include <qmenu.h>

#include <math.h>

TableView::TableView()
: QDialog()
{
	qDebug("qt:startup");

	m_root = NULL;
	m_oldmode = displayseats;

	m_view = new QScrollArea();

	KSeparator *sep1 = new KSeparator();
	KSeparator *sep2 = new KSeparator();

	QLabel *label = new QLabel(i18n("Display:"));

	KComboBox *combo = new KComboBox();
	combo->addItem(i18n("Involved players and bots"), displayseats);
	combo->addItem(i18n("Spectators"), displayspectators);

	QPushButton *ok = new QPushButton(i18n("Close"));

	QVBoxLayout *box = new QVBoxLayout();
	box->addWidget(m_view);
	box->addWidget(sep1);
	QHBoxLayout *box2 = new QHBoxLayout();
	box2->addWidget(label);
	box2->addWidget(combo);
	box2->addStretch(1);
	box->addLayout(box2);
	box->addWidget(sep2);
	box->addWidget(ok);
	setLayout(box);

	QObject::connect(combo, SIGNAL(activated(int)), SLOT(slotDisplay(int)));
	QObject::connect(ok, SIGNAL(clicked()), SLOT(close()));

	setWindowTitle(i18n("Table: Players, Bots and Spectators"));
	resize(400, 300);
	show();
}

void TableView::displaySeats()
{
	qDebug("qt:display seats");

	QPalette palette;
	int count = m_players.count();
	int digits = (int)(log((double)count) / log((double)10.0) + 1);

	qDebug("qt:number of players: %i", count);

	m_root = new QWidget();
	m_view->setWidget(m_root);
	QVBoxLayout *vboxmain = new QVBoxLayout();
	m_root->setLayout(vboxmain);

	KStandardDirs d;

	for(int i = 0; i < count; i++)
	{
		KGGZCore::Player p = m_players.at(i);

		QFrame *w = new QFrame();
		w->setFrameStyle(QFrame::Panel | QFrame::Raised);
		vboxmain->addWidget(w);

		QLCDNumber *numberframe = new QLCDNumber();
		numberframe->setNumDigits(digits);
		numberframe->display(i + 1);

		QFrame *photoframe = new QFrame();
		palette = photoframe->palette();
		palette.setColor(photoframe->backgroundRole(), QColor(120, 120, 120));
		photoframe->setPalette(palette);
		photoframe->setFrameStyle(QFrame::Panel | QFrame::Sunken);
		photoframe->setFixedSize(64, 64);

		QString pixmap = "ox-";

		QString type = "unknown";
		switch(p.type())
		{
			case KGGZCore::Player::player:
				type = i18n("Player");
				pixmap = "player";
				break;
			case KGGZCore::Player::open:
				type = i18n("Open");
				pixmap = "empty";
				break;
			case KGGZCore::Player::reserved:
				type = i18n("Reserved");
				pixmap = "empty";
				break;
			case KGGZCore::Player::bot:
				type = i18n("AI bot");
				pixmap = "bot";
				break;
			case KGGZCore::Player::abandoned:
				type = i18n("Abandoned");
				pixmap = "empty";
				break;
			case KGGZCore::Player::none:
			default:
				break;
		}
		QLabel *typelabel = new QLabel(i18n("Type: %1", type));

		QPixmap pix = d.findResource("data", "kggzlib/players/ox-" + pixmap + ".png");
		QPixmap pixscaled = pix.scaled(QSize(64, 64), Qt::KeepAspectRatio);
		photoframe->show();
		QPalette palette = photoframe->palette();
		palette.setBrush(photoframe->backgroundRole(), QBrush(pixscaled));
		photoframe->setPalette(palette);

		QString name = p.name();
	       	if(name.isNull()) name = i18n("(unnamed)");
		QLabel *namelabel = new QLabel("<b><i>" + name + "</i></b>");
		palette = namelabel->palette();
		palette.setColor(namelabel->backgroundRole(), QColor(255, 255, 255));
		namelabel->setPalette(palette);
		namelabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);
		namelabel->setMinimumWidth(150);

		QVBoxLayout *box = new QVBoxLayout();
		w->setLayout(box);
		QHBoxLayout *box2 = new QHBoxLayout();
		box2->addSpacing(5);
		QVBoxLayout *box5 = new QVBoxLayout();
		box5->addWidget(numberframe);
		box5->addStretch(1);
		box2->addLayout(box5);
		box2->addSpacing(5);
		QVBoxLayout *box4 = new QVBoxLayout();
		box4->addWidget(photoframe);
		box4->addStretch(1);
		box2->addLayout(box4);
		box2->addSpacing(5);
		QVBoxLayout *box3 = new QVBoxLayout();
		box3->addWidget(namelabel);
		box3->addSpacing(5);
		box3->addWidget(typelabel);
		box3->addStretch(1);
		box2->addLayout(box3);
		box2->addStretch(1);
		box->addLayout(box2);

		w->show();
	}

	vboxmain->addStretch(1);

	m_root->show();
	m_root->adjustSize();
}

void TableView::displaySpectators()
{
	qDebug("qt:display spectators");

	int count = m_spectators.count();
	int digits = (int)(log((double)count) / log((double)10.0) + 1);

	m_root = new QWidget();
	m_view->setWidget(m_root);
	QVBoxLayout *vboxmain = new QVBoxLayout();
	m_root->setLayout(vboxmain);

	for(int i = 0; i < count; i++)
	{
		QString spectator = m_spectators.at(i);

		QFrame *w = new QFrame();
		w->setFrameStyle(QFrame::Panel | QFrame::Raised);
		vboxmain->addWidget(w);

		QLCDNumber *numberframe = new QLCDNumber();
		numberframe->setNumDigits(digits);
		numberframe->display(i + 1);

		QString type = i18n("Spectator");
		QLabel *typelabel = new QLabel(i18n("Type: %1", type));

		QString name = spectator;
	       	if(name.isNull()) name = i18n("(unnamed)");
		QLabel *namelabel = new QLabel("<b><i>" + name + "</i></b>");
		QPalette palette = namelabel->palette();
		palette.setColor(namelabel->backgroundRole(), QColor(255, 255, 255));
		namelabel->setPalette(palette);
		namelabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);
		namelabel->setMinimumWidth(150);

		QVBoxLayout *box = new QVBoxLayout();
		w->setLayout(box);
		QHBoxLayout *box2 = new QHBoxLayout();
		box2->addSpacing(5);
		QVBoxLayout *box5 = new QVBoxLayout();
		box5->addWidget(numberframe);
		box5->addStretch(1);
		box2->addLayout(box5);
		box2->addSpacing(5);
		QVBoxLayout *box3 = new QVBoxLayout();
		box3->addWidget(namelabel);
		box3->addSpacing(5);
		box3->addWidget(typelabel);
		box3->addStretch(1);
		box2->addLayout(box3);
		box2->addStretch(1);
		box->addLayout(box2);

		w->show();
	}

	vboxmain->addStretch(1);

	m_root->show();
	m_root->adjustSize();
}

void TableView::addPlayer(KGGZCore::Player player)
{
	m_players.append(player);
	displaySeats();
}

void TableView::addSpectator(QString spectator)
{
	m_spectators.append(spectator);
}

void TableView::slotDisplay(int id)
{
	if(id == m_oldmode) return;
	m_oldmode = id;

	if(id == displayseats) displaySeats();
	else if(id == displayspectators) displaySpectators();
}
