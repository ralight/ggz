#include "kggz_preferences.h"
#include "kggz_settings.h"
#include "kggz_profiles.h"
#include <qpushbutton.h>
#include <qlayout.h>

QHBoxLayout *hbox1;
QWidget *dummy;
KGGZ_Settings *settings;
KGGZ_Profiles *profiles;

KGGZ_Preferences::KGGZ_Preferences(QWidget *parent, char *name)
: QWidget(parent, name)
{
	QListView *preferences_list;
	QPushButton *button_ok;
	QVBoxLayout *vbox1;

	preferences_list = new QListView(this);
	preferences_list->addColumn("Preferences");
	preferences_list->insertItem(new QListViewItem(preferences_list, "Server profiles"));
	preferences_list->insertItem(new QListViewItem(preferences_list, "Settings"));
	preferences_list->setRootIsDecorated(TRUE);
	preferences_list->setFixedWidth(100);

	button_ok = new QPushButton("OK", this);

	dummy = new QWidget(this);

	settings = new KGGZ_Settings(this, NULL);
	settings->hide();

	profiles = new KGGZ_Profiles(this, NULL);
	profiles->hide();

	hbox1 = new QHBoxLayout(this, 5);

	vbox1 = new QVBoxLayout(hbox1, 5);
	vbox1->add(preferences_list);
	vbox1->add(button_ok);

	hbox1->add(dummy);
	hbox1->add(settings);
	hbox1->add(profiles);

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
	if(strcmp(item->text(0), "Settings") == 0)
	{
		dummy->hide();
		profiles->hide();
		settings->show();
	}
	if(strcmp(item->text(0), "Server profiles") == 0)
	{
		dummy->hide();
		settings->hide();
		profiles->show();
	}
}
