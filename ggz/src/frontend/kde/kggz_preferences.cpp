#include "kggz_preferences.h"
#include "kggz_settings.h"
#include "kggz_profiles.h"
#include "kggz_games.h"
#include <qpushbutton.h>
#include <qlayout.h>
#include <qlabel.h>
#include <klocale.h>

QWidget *dummy;
KGGZ_Settings *settings;
KGGZ_Profiles *profiles;
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
	preferences_list->insertItem(new QListViewItem(preferences_list, i18n("Server profiles")));
	preferences_list->insertItem(new QListViewItem(preferences_list, i18n("Settings")));
	preferences_list->insertItem(new QListViewItem(preferences_list, i18n("Games")));
	preferences_list->setRootIsDecorated(TRUE);
	preferences_list->setFixedWidth(100);

	preferences_label = new QLabel(i18n("<font size=+1><b><i><font color=#0000ff>Preferences</font></i></b></font>"), this);

	button_ok = new QPushButton("OK", this);

	dummy = new QWidget(this);
	vbox2 = new QVBoxLayout(dummy, 5);
	preferences_text = new QLabel(i18n("This is the control center of KGGZ. It allows you to set everything up, and check the changes<br>"
		"that have been made in other places, e.g. the connection dialog. There exists a global configuration file, and a local<br>"
		"one for each user. You are free to change the look and feel of KGGZ like you want to."), dummy);
	vbox2->add(preferences_text);

	settings = new KGGZ_Settings(this, NULL);
	settings->hide();

	profiles = new KGGZ_Profiles(this, NULL);
	profiles->hide();

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
	vbox3->add(profiles);
	vbox3->add(games);

	connect(button_ok, SIGNAL(clicked()), SLOT(close()));
	connect(preferences_list, SIGNAL(clicked(QListViewItem *, const QPoint&, int)),
		SLOT(action(QListViewItem *, const QPoint&, int)));

	resize(500, 300);
	setCaption("Preferences");
}

KGGZ_Preferences::~KGGZ_Preferences()
{
}

void KGGZ_Preferences::action(QListViewItem *item, const QPoint &point, int column)
{
	if(item == NULL) return;
	preferences_label->setText("<font size=+1><b><i><font color=#0000ff>" + item->text(0) + "</font></i></b></font>");
	dummy->hide();
	settings->hide();
	profiles->hide();
	games->hide();
	if(strcmp(item->text(0), i18n("Settings")) == 0) settings->show();
	if(strcmp(item->text(0), i18n("Server profiles")) == 0) profiles->show();
	if(strcmp(item->text(0), i18n("Games")) == 0) games->show();
}
