// Header file
#include "tableconfiguration.h"

// KGGZ includes
#include "util.h"

// KDE includes
#include <klocale.h>
#include <kmessagebox.h>
#include <kstandarddirs.h>
#include <kinputdialog.h>

// Qt includes
#include <qlayout.h>
#include <qstring.h>
#include <qslider.h>
#include <qmenu.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qimage.h>
#include <QPixmap>
#include <qstandarditemmodel.h>
#include <qtreeview.h>

static Qt::ItemFlags ROFLAGS =
	Qt::ItemIsSelectable |
	Qt::ItemIsDragEnabled |
	Qt::ItemIsDropEnabled |
	Qt::ItemIsUserCheckable |
	Qt::ItemIsEnabled;

TableConfiguration::TableConfiguration(QWidget *parent)
: QDialog(parent)
{
	QVBoxLayout *vbox;
	QHBoxLayout *hbox;
	QLabel *label2, *label3;
	QPushButton *cancel;

	m_seat = -1;

	m_slider = new QSlider();
	m_slider->setOrientation(Qt::Horizontal);
	m_slider->setMinimum(0);
	m_slider->setMaximum(0);
	m_slider->setTickInterval(1);
	m_slider->setTickPosition(QSlider::TicksBelow);
	m_slider->setPageStep(1);
	m_slider->setEnabled(false);

	m_model = new QStandardItemModel();
	m_model->setColumnCount(3);
	m_model->setHeaderData(0, Qt::Horizontal, i18n("Seat"), Qt::DisplayRole);
	m_model->setHeaderData(1, Qt::Horizontal, i18n("Player"), Qt::DisplayRole);
	m_model->setHeaderData(2, Qt::Horizontal, i18n("Reservation"), Qt::DisplayRole);

	m_seats = new QTreeView();
	m_seats->setRootIsDecorated(false);
	m_seats ->setContextMenuPolicy(Qt::CustomContextMenu);

	m_seats->setModel(m_model);

	m_edit = new QLineEdit();

	m_label = new QLabel(QString());
	label2 = new QLabel(i18n("Seat assignments (right-click to change):"));
	label3 = new QLabel(i18n("Game description:"));

	m_ok = new QPushButton(i18n("Create table"));
	m_ok->setEnabled(false);
	cancel = new QPushButton(i18n("Cancel"));

	vbox = new QVBoxLayout();
	setLayout(vbox);
	vbox->addWidget(m_label);
	vbox->addWidget(m_slider);
	vbox->addWidget(label2);
	vbox->addWidget(m_seats);
	vbox->addWidget(label3);
	vbox->addWidget(m_edit);

	hbox = new QHBoxLayout();
	hbox->addWidget(m_ok);
	hbox->addWidget(cancel);

	vbox->addLayout(hbox);

	connect(m_seats,
		SIGNAL(customContextMenuRequested(const QPoint&)),
		SLOT(slotSelected(const QPoint&)));
	connect(m_slider, SIGNAL(valueChanged(int)), SLOT(slotChanged(int)));
	connect(m_ok, SIGNAL(clicked()), SLOT(slotAccepted()));
	connect(cancel, SIGNAL(clicked()), SLOT(reject()));

	resize(450, 450);
	setWindowTitle(i18n("Launch a game"));
	show();
}

TableConfiguration::~TableConfiguration()
{
}

void TableConfiguration::slotSelected(const QPoint& pos)
{
	QModelIndex index = m_seats->indexAt(pos);
	if(!index.isValid())
		return;

	int seat = index.row();
	//QStandardItem *item_seat = m_model->item(seat, 0);
	//QStandardItem *item_player = m_model->item(seat, 1);
	//QStandardItem *item_reservation = m_model->item(seat, 2);

	if(seat == 0)
		return;
	if(seat >= m_curplayers)
		return;

	QAction *action_seatbot = NULL;
	QAction *action_seatopen = NULL;
	QAction *action_reserved = NULL;
	QList<QAction*> actions_namedbots;
	QList<QAction*> actions_buddies;

	QMenu popup(this);
	action_seatopen = popup.addAction(i18n("Open seat"));
	if((m_curbots < m_maxbots) || ((m_curbots = m_maxbots) && (m_maxbots > 0)))
		action_seatbot = popup.addAction(i18n("Bot player"));

	popup.addSeparator();
	QMenu *reservemenu = popup.addMenu(i18n("Reservation"));
	action_reserved = reservemenu->addAction(i18n("Player"));

	if((m_namedbots.size() > 0) || (m_grubbies.size() > 0))
	{
		QMenu *botmenu = reservemenu->addMenu(i18n("Individual bot"));

		KStandardDirs d;
		QPixmap grubbypix = QPixmap(d.findResource("data", "kggzlib/players/botgrubby.png"));
		QPixmap botpix = QPixmap(d.findResource("data", "kggzlib/players/bot.png"));

		for(int i = 0; i < m_grubbies.size(); i++)
		{
			QString grubbyname = m_grubbies.at(i);
			if(!m_namedbots_reserved[grubbyname])
			{
				QAction *action = botmenu->addAction(grubbypix, grubbyname);
				actions_namedbots.append(action);
			}
		}

		QMap<QString, QString>::const_iterator it = m_namedbots.begin();
		while(it != m_namedbots.end())
		{
			QString botname = it.key();
			QString botclass = it.value();
			if(!m_namedbots_reserved[botname])
			{
				QAction *action = botmenu->addAction(botpix, QString("%1 (%2)").arg(botname).arg(botclass));
				action->setData(botname);
				actions_namedbots.append(action);
			}
			it++;
		}
	}

	if(m_buddies.size() > 0)
	{
		QMenu *buddymenu = reservemenu->addMenu(i18n("Buddy"));

		KStandardDirs d;
		QPixmap pix = QPixmap(d.findResource("data", "kggzlib/players/player.png"));
		QPixmap pix2 = QPixmap(d.findResource("data", "kggzlib/players/buddy.png"));
		QPixmap pix3 = Util::composite(pix, pix2);

		for(int i = 0; i < m_buddies.size(); i++)
		{
			QString buddyname = m_buddies.at(i);
			if(!m_buddies_reserved[buddyname])
			{
				QAction *action = buddymenu->addAction(pix3, buddyname);
				actions_buddies.append(action);
			}
		}
	}

	QAction *action = popup.exec(m_seats->mapToGlobal(pos));

	freeReservation(seat, m_reservations[seat], seatType(seat));

	if(action == action_seatbot)
	{
		setSeatType(seat, seatbot);
	}
	else if(action == action_seatopen)
	{
		setSeatType(seat, seatopen);
	}
	else if(action == action_reserved)
	{
		QString str = KInputDialog::getText(i18n("Reservation"), i18n("Name of the player:"));
		if(!str.isEmpty())
		{
			if(m_players_reserved.contains(str))
			{
				KMessageBox::error(this,
					i18n("The player %1 is already on a seat.", str),
					i18n("Double assignment"));
				return;
			}
			addReservation(seat, str, seatplayerreserved);
		}
	}
	else if(actions_namedbots.contains(action))
	{
		addReservation(seat, action->data().toString(), seatbotreserved);
	}
	else if(actions_buddies.contains(action))
	{
		addReservation(seat, action->text(), seatbuddyreserved);
	}
}

void TableConfiguration::slotAccepted()
{
	m_ok->setEnabled(false);
	accept();
}

void TableConfiguration::slotChanged(int value)
{
	QString str, str2;

	m_curplayers = value;
	str.setNum(value);
	str2.setNum(m_slider->maximum());
	m_label->setText(i18n("Number of players: %1 (out of %2)", str, str2));

	if(m_assignment.size() > 0)
	{
		for(int i = value; i < m_slider->maximum(); i++)
			setSeatType(i, seatunused);
		for(int i = 0; i < value; i++)
			if(seatType(i) == seatunused)
				setSeatType(i, seatopen);
	}
}

QString TableConfiguration::description()
{
	return m_edit->text();
}

int TableConfiguration::seats()
{
	return m_slider->value();
}

void TableConfiguration::initLauncher(QString playername, int maxplayers, int maxbots)
{
	if(m_array.size() > 0)
		return;

	m_curplayers = maxplayers;
	m_maxbots = maxbots;
	m_curbots = maxbots;

	m_playername = playername;

	m_array.resize(maxplayers);
	m_assignment.resize(maxplayers);

	for(int i = 0; i < maxplayers; i++)
	{
		QString str;
		str.setNum(i + 1);
		QStandardItem *item_seat = new QStandardItem();
		item_seat->setText(str);
		item_seat->setFlags(ROFLAGS);
		QStandardItem *item_player = new QStandardItem();
		item_player->setFlags(ROFLAGS);
		QStandardItem *item_reservation = new QStandardItem();
		item_reservation->setFlags(ROFLAGS);
		QList<QStandardItem*> items;
		items << item_seat << item_player << item_reservation;
		m_model->appendRow(items);
	}

	m_slider->setMinimum(2);
	m_slider->setMaximum(maxplayers);
	m_slider->setValue(maxplayers);
	m_slider->setEnabled(true);

	addReservation(0, playername, seatplayer);
	for(int i = 1; i < maxplayers; i++)
	{
		if(i <= maxbots)
			setSeatType(i, seatbot);
		else
			setSeatType(i, seatopen);
	}

	m_ok->setEnabled(true);
}

TableConfiguration::SeatTypes TableConfiguration::seatType(int seat)
{
	SeatTypes ret;

	if((seat < 0) || (seat >= m_array.size()))
		return seatunknown;

	ret = (SeatTypes)m_array.at(seat);

	return ret;
}

void TableConfiguration::setSeatAssignment(int seat, int enabled)
{
	if((seat < 0) || (seat >= m_assignment.size()))
		return;

	m_assignment[seat] = enabled;
}

void TableConfiguration::setSeatType(int seat, SeatTypes seattype)
{
	QString pixmap;
	KStandardDirs d;

	if((seat < 0) || (seat >= m_array.size()))
		return;

	//QStandardItem *item_seat = m_model->item(seat, 0);
	QStandardItem *item_player = m_model->item(seat, 1);
	QStandardItem *item_reservation = m_model->item(seat, 2);

	SeatTypes oldtype = seatType(seat);

	item_player->setText(typeName(seattype));
	item_reservation->setText(m_reservations[seat]);

	if(seattype == seatopen)
	{
		pixmap = "guest.png";
	}
	else if(seattype == seatbot)
	{
		pixmap = "bot.png";
	}
	else if(seattype == seatplayerreserved)
	{
		pixmap = "player.png";
	}
	else if(seattype == seatbotreserved)
	{
		pixmap = "bot.png";
		if(m_reservations.contains(seat))
		{
			if(m_grubbies.contains(m_reservations[seat]))
			{
				pixmap = "botgrubby.png";
				item_player->setText(i18n("Chat bot"));
			}
		}
	}

	QPixmap pix = QPixmap(d.findResource("data", "kggzlib/players/" + pixmap));
	item_player->setIcon(pix);

	if(seattype == seatplayer)
	{
		QPixmap pix1 = QPixmap(d.findResource("data", "kggzlib/players/player.png"));
		QPixmap pix2 = QPixmap(d.findResource("data", "kggzlib/players/you.png"));
		QPixmap pix3 = Util::composite(pix1, pix2);
		item_player->setIcon(pix3);
	}
	else if(seattype == seatbuddyreserved)
	{
		QPixmap pix1 = QPixmap(d.findResource("data", "kggzlib/players/player.png"));
		QPixmap pix2 = QPixmap(d.findResource("data", "kggzlib/players/buddy.png"));
		QPixmap pix3 = Util::composite(pix1, pix2);
		item_player->setIcon(pix3);
	}
	else if(seattype == seatunused)
	{
		QPixmap pix = QPixmap(d.findResource("data", "kggzlib/players/player.png"));
		pix = greyscale(pix);
		item_player->setIcon(pix);
	}

	if(seattype == seatbuddyreserved)
	{
		seattype = seatplayerreserved;
	}
	else if(seattype == seatbotreserved)
	{
		if(m_grubbies.contains(m_reservations[seat]))
			seattype = seatplayerreserved;
		else
			seattype = seatbot;
	}

	if((seattype == seatbot) && (oldtype != seatbot))
		m_curbots++;
	else if((oldtype == seatbot) && (seattype != seatbot))
		m_curbots--;

	m_array[seat] = seattype;
}

void TableConfiguration::addReservation(int seat, QString name, SeatTypes seattype)
{
	if(seattype == seatbuddyreserved)
		m_buddies_reserved[name] = true;
	else if(seattype == seatbotreserved)
		m_namedbots_reserved[name] = true;
	else if(seattype == seatplayerreserved)
		m_players_reserved[name] = true;

	m_reservations[seat] = name;

	setSeatType(seat, seattype);
}

void TableConfiguration::freeReservation(int seat, QString name, SeatTypes seattype)
{
	Q_UNUSED(seattype);

	// FIXME: doesn't differentiate between types yet
	m_buddies_reserved[name] = false;
	m_namedbots_reserved[name] = false;
	m_players_reserved[name] = false;

	m_reservations[seat] = QString();
}

QString TableConfiguration::typeName(SeatTypes seattype)
{
	QString ret;

	switch(seattype)
	{
		case seatplayer:
			ret = i18n("(Yourself)");
			break;
		case seatopen:
			ret = i18n("Open");
			break;
		case seatbot:
			ret = i18n("Bot");
			break;
		case seatplayerreserved:
			ret = i18n("Reserved");
			break;
		case seatunused:
			ret = i18n("(unused)");
			break;
		case seatbuddyreserved:
			ret = i18n("Reserved for buddy");
			break;
		case seatbotreserved:
			ret = i18n("Indidivudal bot");
			break;
		default:
			ret = i18n("unknown");
	}

	return ret;
}

QString TableConfiguration::reservation(int seat)
{
	if((seat < 0) || (seat >= m_array.size()))
		return QString();

	QStandardItem *item_reservation = m_model->item(seat, 2);

	return item_reservation->text();
}

void TableConfiguration::addBot(QString botname, QString botclass)
{
	m_namedbots[botname] = botclass;
}

void TableConfiguration::addBuddy(QString buddyname)
{
	m_buddies.append(buddyname);
}

void TableConfiguration::addGrubby(QString grubbyname)
{
	m_grubbies.append(grubbyname);
}

// FIXME: use kimagefx?
QPixmap TableConfiguration::greyscale(QPixmap orig)
{
	QPixmap comp;
	QRgb rgb;

	QImage origim = orig.toImage();

	for(int j = 0; j < orig.height(); j++)
	{
		for(int i = 0; i < orig.width(); i++)
		{
			rgb = origim.pixel(i, j);
			if(qAlpha(rgb))
			{
				int mix = (qRed(rgb) + qGreen(rgb) + qBlue(rgb)) / 3;
				origim.setPixel(i, j, qRgb(mix, mix, mix));
			}
		}
	}

	comp = comp.fromImage(origim);

	return comp;
}
