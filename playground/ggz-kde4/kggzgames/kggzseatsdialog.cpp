#include "kggzseatsdialog.h"

#include <kggzmod/module.h>
#include <kggzmod/player.h>
#include <kggzmod/statistics.h>

#include <kdebug.h>
#include <klocale.h>
#include <kseparator.h>
#include <kcombobox.h>
#include <kio/job.h>

#include <qlayout.h>
#include <qframe.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qlcdnumber.h>
#include <qscrollarea.h>
#include <qimage.h>
#include <qmenu.h>
#include <qtoolbutton.h>

#include <math.h>

KGGZSeatsDialog::KGGZSeatsDialog(QWidget *parent)
: QWidget(parent)
{
	m_root = NULL;
	m_mod = NULL;
	m_oldmode = displayseats;

	m_view = new QScrollArea();
	//m_view->setResizePolicy(QScrollArea::AutoOneFit);

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

	connect(combo, SIGNAL(activated(int)), SLOT(slotDisplay(int)));
	connect(ok, SIGNAL(clicked()), SLOT(close()));

	setWindowTitle(i18n("Players, Bots and Spectators"));
	resize(300, 300);
	show();
}

KGGZSeatsDialog::~KGGZSeatsDialog()
{
}

void KGGZSeatsDialog::setMod(KGGZMod::Module *mod)
{
	m_mod = mod;

	KGGZMod::InfoRequest ir;
	mod->sendRequest(ir);

	connect(mod, SIGNAL(signalEvent(const KGGZMod::Event&)), SLOT(slotInfo(const KGGZMod::Event&)));

	displaySeats();
}

void KGGZSeatsDialog::displaySeats()
{
	QPalette palette;
	int count = m_mod->players().count();
	int digits = (int)(log(count) / log(10) + 1);

	if(m_root)
	{
		// FIXME: m_root doesn't need to be member anymore? (Qt4)
		(void)m_view->takeWidget();
		delete m_root;

		m_hostnames.clear();
		m_realnames.clear();
		m_photos.clear();
		m_buttons.clear();
		m_buttondata.clear();
	}
	m_root = new QWidget(m_view->viewport());
	m_view->setWidget(m_root);
	QVBoxLayout *vboxmain = new QVBoxLayout(m_root);

	for(int i = 0; i < count; i++)
	{
		KGGZMod::Player *p = m_mod->players().at(i);

		QFrame *w = new QFrame(m_root);
		w->setFrameStyle(QFrame::Panel | QFrame::Raised);
		vboxmain->addWidget(w);

		QLCDNumber *numberframe = new QLCDNumber(w);
		numberframe->setNumDigits(digits);
		numberframe->display(i + 1);

		QFrame *photoframe = new QFrame(w);
		palette = photoframe->palette();
		palette.setColor(photoframe->backgroundRole(), QColor(120, 120, 120));
		photoframe->setPalette(palette);
		photoframe->setFrameStyle(QFrame::Panel | QFrame::Sunken);
		photoframe->setFixedSize(64, 64);

		QToolButton *actionbutton = new QToolButton(w);
		actionbutton->setArrowType(Qt::DownArrow);
		actionbutton->setText(i18n("Action..."));

		QString type = "unknown";
		switch(p->type())
		{
			case KGGZMod::Player::player:
				type = i18n("Player");
				break;
			case KGGZMod::Player::open:
				type = i18n("Open");
				break;
			case KGGZMod::Player::reserved:
				type = i18n("Reserved");
				break;
			case KGGZMod::Player::bot:
				type = i18n("AI bot");
				break;
			case KGGZMod::Player::abandoned:
				type = i18n("Abandoned");
				break;
			case KGGZMod::Player::unknown:
			default:
				break;
		}
		QLabel *typelabel = new QLabel(i18n("Type: %1").arg(type), w);

		QString name = p->name();
	       	if(name.isNull()) name = i18n("(unnamed)");
		QLabel *namelabel = new QLabel("<b><i>" + name + "</i></b>", w);
		palette = namelabel->palette();
		palette.setColor(namelabel->backgroundRole(), QColor(255, 255, 255));
		namelabel->setPalette(palette);
		namelabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);
		namelabel->setMinimumWidth(150);

		QLabel *hostlabel = new QLabel("", w);
		hostlabel->hide();

		QLabel *realnamelabel = new QLabel("", w);
		realnamelabel->hide();

		m_hostnames[i] = hostlabel;
		m_realnames[i] = realnamelabel;
		m_photos[i] = photoframe;

		QVBoxLayout *box = new QVBoxLayout(w);
		QHBoxLayout *box2 = new QHBoxLayout();
		box2->addSpacing(5);
		QVBoxLayout *box5 = new QVBoxLayout();
		box5->addWidget(numberframe);
		box5->addWidget(actionbutton);
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
		box3->addSpacing(5);
		box3->addWidget(realnamelabel);
		box3->addSpacing(5);
		box3->addWidget(hostlabel);
		box3->addStretch(1);
		box2->addLayout(box3);
		box2->addStretch(1);
		box->addLayout(box2);

		m_buttons[actionbutton] = i;
		m_buttondata[actionbutton] = actionbutton;

		connect(actionbutton, SIGNAL(clicked()), SLOT(slotAction()));
	}

	vboxmain->addStretch(1);

	m_root->show();

	infos();
}

void KGGZSeatsDialog::displaySpectators()
{
	int count = m_mod->spectators().count();
	int digits = (int)(log(count) / log(10) + 1);

	if(m_root)
	{
		(void)m_view->takeWidget();
		delete m_root;
	}
	m_root = new QWidget(m_view->viewport());
	m_view->setWidget(m_root);
	QVBoxLayout *vboxmain = new QVBoxLayout(m_root);

	for(int i = 0; i < count; i++)
	{
		KGGZMod::Player *p = m_mod->spectators().at(i);

		QFrame *w = new QFrame(m_root);
		w->setFrameStyle(QFrame::Panel | QFrame::Raised);
		vboxmain->addWidget(w);

		QLCDNumber *numberframe = new QLCDNumber(w);
		numberframe->setNumDigits(digits);
		numberframe->display(i + 1);

		QString type = i18n("Spectator");
		QLabel *typelabel = new QLabel(i18n("Type: %1").arg(type), w);

		QString name = p->name();
	       	if(name.isNull()) name = i18n("(unnamed)");
		QLabel *namelabel = new QLabel("<b><i>" + name + "</i></b>", w);
		QPalette palette = namelabel->palette();
		palette.setColor(namelabel->backgroundRole(), QColor(255, 255, 255));
		namelabel->setPalette(palette);
		namelabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);
		namelabel->setMinimumWidth(150);

		QVBoxLayout *box = new QVBoxLayout(w);
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
	}

	vboxmain->addStretch(1);

	m_root->show();
}

void KGGZSeatsDialog::slotAction()
{
	int seat;
       
	if(m_buttons.contains(sender()))
	{
		seat = m_buttons[sender()];
		kdDebug() << "seat " << seat << " oldmode " << m_oldmode << endl;

		KGGZMod::Player *p = m_mod->players().at(seat);
		KGGZMod::Player *pself = m_mod->self();
		m_currentplayer = p;

		QMenu *pop = new QMenu(this);
		action_viewstats = pop->addAction(i18n("Statistics..."));
		pop->addSeparator();
		if(p->type() == KGGZMod::Player::open)
		{
			if(pself->type() == KGGZMod::Player::spectator)
			{
				action_sitdown = pop->addAction(i18n("Sit down here"));
			}
			action_botadd = pop->addAction(i18n("Add a bot here"));
		}
		else if(p->type() == KGGZMod::Player::bot)
		{
			action_botremove = pop->addAction(i18n("Boot bot and open seat"));
		}
		else if(p->type() == KGGZMod::Player::player)
		{
			if(pself->type() == KGGZMod::Player::player)
			{
				action_standup = pop->addAction(i18n("Stand up"));
			}
			action_bootplayer = pop->addAction(i18n("Boot player and open seat"));
		}
		else if(p->type() == KGGZMod::Player::reserved)
		{
		}
		else if(p->type() == KGGZMod::Player::abandoned)
		{
		}

		connect(pop, SIGNAL(triggered(QAction*)), SLOT(slotMenu(QAction*)));

		const QToolButton *buttonkey = dynamic_cast<const QToolButton*>(sender());
		QToolButton *button = m_buttondata[buttonkey];
		//button->setPopup(pop);
		//button->openPopup();
		pop->popup(button->mapToGlobal(QPoint(0, 0)));
	}
	else
	{
		kdDebug() << "error" << endl;
		// error!
	}
}

void KGGZSeatsDialog::slotMenu(QAction *action)
{
	kdDebug() << "slotMenu! action=" << action->text() << endl;

	if(action == action_standup)
	{
		KGGZMod::StandRequest req;
		m_mod->sendRequest(req);
	}
	else if(action == action_sitdown)
	{
		KGGZMod::SitRequest req(m_currentplayer->seat());
		m_mod->sendRequest(req);
	}
	else if(action == action_bootplayer)
	{
		KGGZMod::BootRequest req(m_currentplayer->name());
		m_mod->sendRequest(req);
	}
	else if(action == action_botadd)
	{
		KGGZMod::BotRequest req(m_currentplayer->seat());
		m_mod->sendRequest(req);
	}
	else if(action == action_botremove)
	{
		KGGZMod::OpenRequest req(m_currentplayer->seat());
		m_mod->sendRequest(req);
	}
	else if(action == action_viewstats)
	{
		// FIXME: how to display stats for all players globally???
		KGGZMod::Statistics *s = m_currentplayer->stats();
		if(s->hasRecord())
		{
			kdDebug() << "Wins: " << s->wins() << endl;
		}
	}
}

void KGGZSeatsDialog::slotInfo(const KGGZMod::Event& event)
{
	if(event.type() == KGGZMod::Event::info)
	{
		infos();
	}
	else if(event.type() == KGGZMod::Event::seat)
	{
			//KGGZMod::SeatEvent se = (KGGZMod::SeatEvent)event;
	}
}

void KGGZSeatsDialog::infos()
{
	int count = m_mod->players().count();
	for(int i = 0; i < count; i++)
	{
		KGGZMod::Player *p = m_mod->players().at(i);
		// FIXME: condition if info is really there? not really needed
		if(/*info*/1==1)
		{
			if(!p->hostname().isEmpty())
			{
				QString hostname = i18n("Host: %1").arg(p->hostname());
				m_hostnames[i]->setText(hostname);
				m_hostnames[i]->show();
			}
			if(!p->realname().isEmpty())
			{
				QString realname = i18n("Realname: %1").arg(p->realname());
				m_realnames[i]->setText(realname);
				m_realnames[i]->show();
			}
			if(!p->photo().isEmpty())
			{
				KIO::TransferJob *job = KIO::get(p->photo(), false, false);
				connect(job, SIGNAL(data(KIO::Job*, const QByteArray&)),
					SLOT(slotTaskData(KIO::Job*, const QByteArray&)));
				connect(job, SIGNAL(result(KIO::Job*)),
					SLOT(slotTaskResult(KIO::Job*)));
				m_phototasks[job] = i;
				m_photodata[job] = QByteArray();
			}
		}
	}
}

void KGGZSeatsDialog::slotDisplay(int id)
{
	if(id == m_oldmode) return;
	m_oldmode = id;

	if(id == displayseats) displaySeats();
	else if(id == displayspectators) displaySpectators();
}

void KGGZSeatsDialog::slotTaskData(KIO::Job *job, const QByteArray& data)
{
	QByteArray data2 = m_photodata[job];
	int origsize = data2.size();

	// FIXME: use Qt4 append function?
	data2.resize(data2.size() + data.size());
	for(int i = 0; i < data.size(); i++)
	{
		data2[origsize + i] = data[i];
	}
	m_photodata[job] = data2;
}

void KGGZSeatsDialog::slotTaskResult(KIO::Job *job)
{
	if(!job->error())
	{
		int i = m_phototasks[job];
		QByteArray data = m_photodata[job];
		QPixmap pix(data);
		/*QImage im = pix.convertToImage();
		QImage im2(64, 64, 32);
		im = im.smoothScale(64, 64, QImage::ScaleMin);
		im2.fill(QColor(0, 0, 0).rgb());
		int x = (64 - im.width()) / 2;
		int y = (64 - im.height()) / 2;
		bitBlt(&im2, x, y, &im, 0, 0, -1, -1, 0);
		pix.convertFromImage(im2);*/
		QPixmap pixscaled = pix.scaled(QSize(64, 64), Qt::KeepAspectRatio);
		QPalette palette = m_photos[i]->palette();
		palette.setBrush(m_photos[i]->backgroundRole(), QBrush(pixscaled));
		m_photos[i]->setPalette(palette);
	}

	m_photodata.remove(job);
	m_phototasks.remove(job);
}

