#include "kggzseatsdialog.h"

#include <kggzmod/module.h>
#include <kggzmod/player.h>

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
#include <qscrollview.h>
#include <qimage.h>

#include <math.h>

KGGZSeatsDialog::KGGZSeatsDialog(QWidget *parent, const char *name)
: QWidget(parent, name)
{
	m_root = NULL;
	m_oldmode = displayseats;

	m_view = new QScrollView(this);
	m_view->setResizePolicy(QScrollView::AutoOneFit);

	KSeparator *sep1 = new KSeparator(this);
	KSeparator *sep2 = new KSeparator(this);

	QLabel *label = new QLabel(i18n("Display:"), this);

	KComboBox *combo = new KComboBox(this);
	combo->insertItem(i18n("Involved players and bots"), displayseats);
	combo->insertItem(i18n("Spectators"), displayspectators);

	QPushButton *ok = new QPushButton(i18n("Close"), this);

	QVBoxLayout *box = new QVBoxLayout(this, 5);
	box->add(m_view);
	box->add(sep1);
	QHBoxLayout *box2 = new QHBoxLayout(box, 5);
	box2->add(label);
	box2->add(combo);
	box2->addStretch(1);
	box->add(sep2);
	box->add(ok);

	connect(combo, SIGNAL(activated(int)), SLOT(slotDisplay(int)));
	connect(ok, SIGNAL(clicked()), SLOT(close()));

	setCaption(i18n("Players, Bots and Spectators"));
	show();
}

KGGZSeatsDialog::~KGGZSeatsDialog()
{
}

void KGGZSeatsDialog::setMod(KGGZMod::Module *mod)
{
	m_mod = mod;

	KGGZMod::InfoRequest ir(-1);
	mod->sendRequest(ir);

	connect(mod, SIGNAL(signalEvent(KGGZMod::Event)), SLOT(slotInfo(KGGZMod::Event)));

	displaySeats();
}

void KGGZSeatsDialog::displaySeats()
{
	//int count = ggzmod_get_num_seats(m_mod);
	// FIXME: filter according to settings?

	int count = m_mod->players().count();
	int digits = (int)(log(count) / log(10) + 1);

	if(m_root)
	{
		m_view->removeChild(m_root);
		delete m_root;

		m_hostnames.clear();
		m_realnames.clear();
		m_photos.clear();
	}
	m_root = new QWidget(m_view->viewport());
	m_view->addChild(m_root);
	QVBoxLayout *vboxmain = new QVBoxLayout(m_root);

	for(int i = 0; i < count; i++)
	{
		KGGZMod::Player *p = *(m_mod->players().at(i));

		QFrame *w = new QFrame(m_root);
		w->setFrameStyle(QFrame::Panel | QFrame::Raised);
		vboxmain->add(w);

		QLCDNumber *numberframe = new QLCDNumber(w);
		numberframe->setNumDigits(digits);
		numberframe->display(i + 1);

		QFrame *photoframe = new QFrame(w);
		photoframe->setBackgroundColor(QColor(120, 120, 120));
		photoframe->setFrameStyle(QFrame::Panel | QFrame::Sunken);
		photoframe->setFixedSize(64, 64);

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
		namelabel->setBackgroundColor(QColor(255, 255, 255));
		namelabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);
		namelabel->setMinimumWidth(150);

		QLabel *hostlabel = new QLabel("", w);
		hostlabel->hide();

		QLabel *realnamelabel = new QLabel("", w);
		realnamelabel->hide();

		m_hostnames[i] = hostlabel;
		m_realnames[i] = realnamelabel;
		m_photos[i] = photoframe;

		QVBoxLayout *box = new QVBoxLayout(w, 5);
		QHBoxLayout *box2 = new QHBoxLayout(box);
		box2->addSpacing(5);
		QVBoxLayout *box5 = new QVBoxLayout(box2);
		box5->add(numberframe);
		box5->addStretch(1);
		box2->addSpacing(5);
		QVBoxLayout *box4 = new QVBoxLayout(box2);
		box4->add(photoframe);
		box4->addStretch(1);
		box2->addSpacing(5);
		QVBoxLayout *box3 = new QVBoxLayout(box2);
		box3->add(namelabel);
		box3->addSpacing(5);
		box3->add(typelabel);
		box3->addSpacing(5);
		box3->add(realnamelabel);
		box3->addSpacing(5);
		box3->add(hostlabel);
		box3->addStretch(1);
		box2->addStretch(1);
	}

	vboxmain->addStretch(1);

	m_root->show();

	infos();
}

void KGGZSeatsDialog::displaySpectators()
{
	//int count = ggzmod_get_num_spectator_seats(m_mod);
	// FIXME: see displaySeats();

	int count = m_mod->players().count();
	int digits = (int)(log(count) / log(10) + 1);

	if(m_root)
	{
		m_view->removeChild(m_root);
		delete m_root;
	}
	m_root = new QWidget(m_view->viewport());
	m_view->addChild(m_root);
	QVBoxLayout *vboxmain = new QVBoxLayout(m_root);

	for(int i = 0; i < count; i++)
	{
		KGGZMod::Player *p = *(m_mod->players().at(i));

		QFrame *w = new QFrame(m_root);
		w->setFrameStyle(QFrame::Panel | QFrame::Raised);
		vboxmain->add(w);

		QLCDNumber *numberframe = new QLCDNumber(w);
		numberframe->setNumDigits(digits);
		numberframe->display(i + 1);

		QString type = i18n("Spectator");
		QLabel *typelabel = new QLabel(i18n("Type: %1").arg(type), w);

		QString name = p->name();
	       	if(name.isNull()) name = i18n("(unnamed)");
		QLabel *namelabel = new QLabel("<b><i>" + name + "</i></b>", w);
		namelabel->setBackgroundColor(QColor(255, 255, 255));
		namelabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);
		namelabel->setMinimumWidth(150);

		QVBoxLayout *box = new QVBoxLayout(w, 5);
		QHBoxLayout *box2 = new QHBoxLayout(box);
		box2->addSpacing(5);
		QVBoxLayout *box5 = new QVBoxLayout(box2);
		box5->add(numberframe);
		box5->addStretch(1);
		box2->addSpacing(5);
		QVBoxLayout *box3 = new QVBoxLayout(box2);
		box3->add(namelabel);
		box3->addSpacing(5);
		box3->add(typelabel);
		box3->addStretch(1);
		box2->addStretch(1);
	}

	vboxmain->addStretch(1);

	m_root->show();
}

void KGGZSeatsDialog::slotInfo(KGGZMod::Event event)
{
	if(event.type() == KGGZMod::Event::info)
	{
		infos();
	}
}

void KGGZSeatsDialog::infos()
{
	//int count = ggzmod_get_num_seats(m_mod);
	// FIXME: See display*
	int count = m_mod->players().count();
	for(int i = 0; i < count; i++)
	{
		KGGZMod::Player *p = *(m_mod->players().at(i));
		// FIXME: condition if info is really there? not really needed
		if(/*info*/1==1)
		{
			if(p->hostname())
			{
				QString hostname = i18n("Host: %1").arg(p->hostname());
				m_hostnames[i]->setText(hostname);
				m_hostnames[i]->show();
			}
			if(p->realname())
			{
				QString realname = i18n("Realname: %1").arg(p->realname());
				m_realnames[i]->setText(realname);
				m_realnames[i]->show();
			}
			if(p->photo())
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

	data2.resize(data2.size() + data.size());
	for(unsigned int i = 0; i < data.size(); i++)
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
		QImage im = pix.convertToImage();
		QImage im2(64, 64, 32);
		im = im.smoothScale(64, 64, QImage::ScaleMin);
		im2.fill(QColor(0, 0, 0).rgb());
		int x = (64 - im.width()) / 2;
		int y = (64 - im.height()) / 2;
		bitBlt(&im2, x, y, &im, 0, 0, -1, -1, 0);
		pix.convertFromImage(im2);
		m_photos[i]->setBackgroundPixmap(pix);
	}

	m_photodata.remove(job);
	m_phototasks.remove(job);
}

