#include "shadowclientggz.h"

#include <qlayout.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <stdlib.h>
#include <string.h>
#include "GGZCoreConfio.h"

ShadowClientGGZ::ShadowClientGGZ(QWidget *parent, const char *name)
: QWidget(parent, name)
{
	QLabel *label;
	QPushButton *ok, *cancel;
	QVBoxLayout *vbox;
	QHBoxLayout *hbox;

	ok = new QPushButton("OK", this);
	cancel = new QPushButton("Cancel", this);

	label = new QLabel("Please select a GGZ game client:", this);

	combo = new KComboBox(this);

	vbox = new QVBoxLayout(this, 5);
	vbox->add(label);
	vbox->add(combo);
	hbox = new QHBoxLayout(vbox, 5);
	hbox->add(ok);
	hbox->add(cancel);

	connect(cancel, SIGNAL(clicked()), SLOT(close()));
	connect(ok, SIGNAL(clicked()), SLOT(slotPressed()));

	init();
}

ShadowClientGGZ::~ShadowClientGGZ()
{
}

void ShadowClientGGZ::init()
{
	// GGZ stuff goes here
	GGZCoreConfio *conf;
	char **list, **felist;
	int count, fecount, ret;
	char *fe;

	conf = new GGZCoreConfio("/etc/ggz.modules", GGZCoreConfio::readonly);
	ret = conf->read("Games", "*Engines*", &count, &list);	
	if(ret == -1) return;

	if(list)
	{
		for(int i = 0; i < count; i++)
		{
			ret = conf->read("Games", list[i], &fecount, &felist);
			if(ret != -1)
			{
				if(felist)
				{
					for(int j = 0; j < fecount; j++)
					{
						fe = conf->read(felist[j], "Frontend", "unknown");
						combo->insertItem(QString("%1 (%2)").arg(list[i]).arg(fe));
						fe = conf->read(felist[j], "CommandLine", "");
						cmdlinelist.append(strdup(fe));
						fe = conf->read(felist[j], "Name", list[i]);
						namelist.append(strdup(fe));
						free(felist[j]);
					}
					free(felist);
				}
			}
			free(list[i]);
		}
		free(list);
	}

	delete conf;
}

void ShadowClientGGZ::slotPressed()
{
	int i;

	i = 0; // QUICK HACK
    emit signalClient(namelist.at(i), cmdlinelist.at(i));
    close();
}

