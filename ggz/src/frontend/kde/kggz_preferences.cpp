#include "kggz_preferences.h"
#include "kggz_settings.h"
#include "kggz_playerinfo.h"
#include "kggz_hosts.h"
#include "kggz_ftp.h"
#include "kggz_games.h"
#include <qpushbutton.h>
#include <qlayout.h>
#include <qlabel.h>
#include <klocale.h>

QWidget *dummy;
KGGZ_Settings *settings;
KGGZ_Playerinfo *playerinfo;
KGGZ_Hosts *hosts;
KGGZ_Ftp *ftp;
KGGZ_Games *games;
QLabel *preferences_label;

KGGZ_Preferences::KGGZ_Preferences(QWidget *parent, char *name)
: QWidget(parent, name)
{
	QListView *preferences_list;
	QPushButton *button_ok;
	QVBoxLayout *vbox1, *vbox2, *vbox3;
	QHBoxLayout *hbox1;
	QLabel *preferences_text;

	preferences_list = new QListView(this);
	preferences_list->addColumn(i18n("Preferences"));
	preferences_list->insertItem(new QListViewItem(preferences_list, i18n("Game Server profiles")));
	preferences_list->insertItem(new QListViewItem(preferences_list, i18n("FTP Server profiles")));
	preferences_list->insertItem(new QListViewItem(preferences_list, i18n("Settings")));
	preferences_list->insertItem(new QListViewItem(preferences_list, i18n("Player information")));
	preferences_list->insertItem(new QListViewItem(preferences_list, i18n("Games")));
	preferences_list->setRootIsDecorated(TRUE);
	preferences_list->setFixedWidth(100);

	preferences_label = new QLabel("", this);

	button_ok = new QPushButton("OK", this);

	dummy = new QWidget(this);
	vbox2 = new QVBoxLayout(dummy, 5);
	preferences_text = new QLabel(i18n("This is the control center of KGGZ. It allows you to set everything up, and check the changes<br>"
		"that have been made in other places, e.g. the connection dialog. There exists a global configuration file, and a local<br>"
		"one for each user. You are free to change the look and feel of KGGZ like you want to."), dummy);
	vbox2->add(preferences_text);

	settings = new KGGZ_Settings(this, NULL);
	settings->hide();

	playerinfo = new KGGZ_Playerinfo(this, NULL);
	playerinfo->hide();

	hosts = new KGGZ_Hosts(this, NULL);
	hosts->hide();

	ftp = new KGGZ_Ftp(this, NULL);
	ftp->hide();

	games = new KGGZ_Games(this, NULL);
	games->hide();

	hbox1 = new QHBoxLayout(this, 5);
	vbox1 = new QVBoxLayout(hbox1, 5);
	vbox1->add(preferences_list);
	vbox1->add(button_ok);

	vbox3 = new QVBoxLayout(hbox1, 5);
	vbox3->setAlignment(Qt::AlignTop);
	vbox3->add(preferences_label);
	vbox3->add(dummy);
	vbox3->add(settings);
	vbox3->add(playerinfo);
	vbox3->add(hosts);
	vbox3->add(ftp);
	vbox3->add(games);

	connect(button_ok, SIGNAL(clicked()), SLOT(close()));
	connect(preferences_list, SIGNAL(clicked(QListViewItem *, const QPoint&, int)),
		SLOT(slotAction(QListViewItem *, const QPoint&, int)));

	slotDefault();

	resize(500, 300);
	setCaption(i18n("Preferences"));
}

KGGZ_Preferences::~KGGZ_Preferences()
{
}

void KGGZ_Preferences::slotDefault()
{
	preferences_label->setText("<font size=+1><b><i><font color=#0000ff>" + i18n("Preferences") + "</font></i></b></font>");
	dummy->show();
}

void KGGZ_Preferences::slotAction(QListViewItem *item, const QPoint &point, int column)
{
	int dummycounter;

	if(item == NULL) return;
	preferences_label->setText("<font size=+1><b><i><font color=#0000ff>" + item->text(0) + "</font></i></b></font>");
	dummy->hide();
	settings->hide();
	playerinfo->hide();
	hosts->hide();
	ftp->hide();
	games->hide();
	if(strcmp(item->text(0), i18n("Settings")) == 0)
	{
		settings->show();
		connect(settings, SIGNAL(signalClosed()), SLOT(slotDefault()));
	}
	if(strcmp(item->text(0), i18n("Player information")) == 0)
	{
		playerinfo->show();
		connect(playerinfo, SIGNAL(signalClosed()), SLOT(slotDefault()));
	}
	if(strcmp(item->text(0), i18n("Game Server profiles")) == 0)
	{
		hosts->show();
		connect(hosts, SIGNAL(signalClosed()), SLOT(slotDefault()));
	}
	if(strcmp(item->text(0), i18n("FTP Server profiles")) == 0)
	{
		ftp->show();
		connect(ftp, SIGNAL(signalClosed()), SLOT(slotDefault()));
	}
	if(strcmp(item->text(0), i18n("Games")) == 0)
	{
		games->show();
		/*connect(games, SIGNAL(ready()), dummy, SIGNAL(show()));*/
	}
}
