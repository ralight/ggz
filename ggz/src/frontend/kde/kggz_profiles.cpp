/* Class declaration */
#include "kggz_profiles.h"

/* Qt classes */
#include <qcombobox.h>
#include <qpushbutton.h>

/* GGZcore for server profiles */
#include <ggzcore.h>

/* System include */
#include <stdlib.h>
#include <stdio.h>

#include <klocale.h>

// FIXME: find a solution. e.g. use "const char" in libggzcore.
// done: now obsolete (but still declaration issue, eh?)
/*char *const2char(const char *s)
{
	static char *p = NULL;

	//if(p != NULL) free(p);
	p = (char*)malloc(strlen(s) + 1);
	strcpy(p, s);
	return p;
}*/

// Constructor
KGGZ_Profiles::KGGZ_Profiles(QWidget *parent, char *name)
: QWidget(parent, name)
{
	QVBoxLayout *vbox1, *vbox2;
	QHBoxLayout *hbox1, *hbox2;

	QPushButton *button_ok, *button_cancel;
	QPushButton *profile_add, *profile_remove;

	m_labellist.setAutoDelete(TRUE);
	m_inputlist.setAutoDelete(TRUE);
	m_entrylist.setAutoDelete(TRUE);
	m_deflist.setAutoDelete(TRUE);

	profiles_list = new QListView(this);

	profile_add = new QPushButton(i18n("Add"), this);
	profile_remove = new QPushButton(i18n("Remove"), this);

	button_ok = new QPushButton("OK", this);
	button_cancel = new QPushButton(i18n("Cancel"), this);

	vbox1 = new QVBoxLayout(this, 5);

	hbox1 = new QHBoxLayout(vbox1, 5);
	hbox1->add(profiles_list);

	vbox2 = new QVBoxLayout(hbox1, 5);
	vbox2->add(profile_add);
	vbox2->add(profile_remove);

	m_listbox = new QHBoxLayout(vbox1, 5);

	hbox2 = new QHBoxLayout(vbox1, 5);
	hbox2->add(button_ok);
	hbox2->add(button_cancel);

	connect(profile_add, SIGNAL(clicked()), SLOT(slotAddProfile()));
	connect(profile_remove, SIGNAL(clicked()), SLOT(slotRemoveProfile()));
	connect(button_cancel, SIGNAL(clicked()), SLOT(slotClose()));
	connect(button_ok, SIGNAL(clicked()), SLOT(slotAccept()));

	resize(400, 200);
}

void KGGZ_Profiles::readEntries()
{
	QLabel *tmp;
	char *m_tmp;
	QListViewItem *tmpitem;
	int i, j, k;
	char **list;
	char *tmp_entry, *tmp_def;
	char *tmp_section;

	list = NULL;
	printf("Read entries via ggzcore\n");
	tmp_section = (char*)malloc(strlen(m_sectionname.latin1()) + 1);
	strcpy(tmp_section, m_sectionname.latin1());
	ggzcore_conf_read_list(tmp_section, tmp_section, &i, &list);
	if(tmp_section) free(tmp_section);
	printf("Read some more (%i entries)\n", i);
	for(j = 0; j < i; j++)
	{
		tmpitem = new QListViewItem(profiles_list);
		tmpitem->setText(0, list[j]);
		k = 1;
		m_entrylist.first();
		m_entrylist.next();
		m_deflist.first();
		m_deflist.next();
		tmp = m_labellist.first();
		tmp = m_labellist.next();
		for(; tmp != NULL; tmp = m_labellist.next())
		{
			tmp_entry = (char*)malloc(strlen(m_entrylist.current()->latin1()) + 1);
			tmp_def = (char*)malloc(strlen(m_deflist.current()->latin1()) + 1);
			sprintf(tmp_entry, m_entrylist.current()->latin1());
			sprintf(tmp_def, m_deflist.current()->latin1());
			printf("** entry: %s/%s/%s: %s\n", list[j], tmp_entry, tmp_def, ggzcore_conf_read_string(list[j], tmp_entry, tmp_def));
			m_tmp = ggzcore_conf_read_string(list[j], tmp_entry, tmp_def);
			tmpitem->setText(k, m_tmp);
			if(tmp_entry) free(tmp_entry);
			if(tmp_def) free(tmp_def);
			m_entrylist.next();
			m_deflist.next();
			k++;
		}
		free(list[j]);
	}
	printf("Free the list\n");
	if(list) free(list);
}

void KGGZ_Profiles::setSectionName(char *name)
{
	m_sectionname = name;
}

void KGGZ_Profiles::addField(char *name, char *entry, char *def)
{
	QVBoxLayout *tmp;

	profiles_list->addColumn(name);

	m_labellist.append(new QLabel(name, this));
	m_inputlist.append(new QLineEdit(this));
	m_entrylist.append(new QString(entry));
	m_deflist.append(new QString(def));

	tmp = new QVBoxLayout(m_listbox, 5);
	tmp->add(m_labellist.current());
	tmp->add(m_inputlist.current());
}

// Destructor
KGGZ_Profiles::~KGGZ_Profiles()
{
}

void KGGZ_Profiles::slotClose()
{
	m_labellist.clear();
	m_inputlist.clear();
	m_entrylist.clear();
	m_deflist.clear();
	printf("Cleared\n");

	emit signalClosed();

	close();
}

void KGGZ_Profiles::slotAccept()
{
	// store configuration values
	//char tmpval[256];
	char **list;
	int i, k;
	char *tmp_section;
	QListViewItem *tmpitem;
	QLabel *tmp;
	char *tmp_key, *tmp_entry;

	printf("Write entries via ggzcore\n");
	printf("Allocating space for %i entries\n", profiles_list->childCount());
	list = (char**)malloc(profiles_list->childCount());
	tmpitem = profiles_list->firstChild();
	i = 0;
	while(tmpitem)
	{
		m_entrylist.first();
		m_deflist.first();

		list[i] = (char*)malloc(strlen(tmpitem->text(0)) + 1);
		strcpy(list[i], tmpitem->text(0));
		printf("Assigning to list: %s\n", list[i]);

		k = 1;
		m_entrylist.first();
		m_entrylist.next();
		tmp = m_labellist.first();
		tmp = m_labellist.next();
		for(; tmp != NULL; tmp = m_labellist.next())
		{
			//printf("Caption :: Text: %s :: %s\n", tmp->caption().latin1(), tmp->text().latin1());
			tmp_section = (char*)malloc(strlen(tmpitem->text(0)) + 1);
			tmp_key = (char*)malloc(strlen(m_entrylist.current()->latin1()) + 1);
			tmp_entry = (char*)malloc(strlen(tmpitem->text(k)) + 1);
			strcpy(tmp_section, tmpitem->text(0));
			strcpy(tmp_key, m_entrylist.current()->latin1());
			strcpy(tmp_entry, tmpitem->text(k));
			ggzcore_conf_write_string(tmp_section, tmp_key, tmp_entry);
			printf("** Writing: %s/%s/%s\n", tmp_section, tmp_key, tmp_entry);
			if(tmp_section) free(tmp_section);
			if(tmp_key) free(tmp_key);
			if(tmp_entry) free(tmp_entry);
			m_entrylist.next();
			//m_deflist.next();
			k++;
		}
		tmpitem = tmpitem->itemBelow();
		i++;
	}

	if(i > 0)
	{
		tmp_section = (char*)malloc(strlen(m_sectionname.latin1()) + 1);
		strcpy(tmp_section, m_sectionname.latin1());
		ggzcore_conf_write_list(tmp_section, tmp_section, i, list);
		if(tmp_section) free(tmp_section);

		printf("Freeing memory...\n");
		for(int j = 0; j < i - 1; j++)
			free(list[j]);
	}
	if(list) free(list);

	printf("Commit changes\n");
	ggzcore_conf_commit();

	// inform parent class about new configuration
	emit signalUpdate(-1);

	slotClose();
}

void KGGZ_Profiles::slotRemoveProfile()
{
	char *section;

	// set up buffer and remove profile
	section = (char*)malloc(strlen(profiles_list->currentItem()->text(0)) + 1);
	strcpy(section, profiles_list->currentItem()->text(0));
	ggzcore_conf_remove_section(section);
	ggzcore_conf_commit();
	free(section);

	// remove from listview
	profiles_list->takeItem(profiles_list->currentItem());
}

void KGGZ_Profiles::slotAddProfile()
{
	QListViewItem *tmpitem;
	/*QLabel *tmp;*/
	QLineEdit *tmpe;
	int k;

	//profiles_list->insertItem(new QListViewItem(profiles_list, input_name->text(), input_host->text(), input_port->text()));
	//input_name->clear();
	//input_host->clear();
	//input_port->clear();

	tmpitem = new QListViewItem(profiles_list);
	k = 0;
	for(/*tmp = m_labellist.first(), */tmpe = m_inputlist.first(); tmpe != NULL;/* tmp = m_labellist.next(),*/ tmpe = m_inputlist.next())
	{
		printf("Adding: %s\n", tmpe->text().latin1());
		tmpitem->setText(k, tmpe->text());
		tmpe->clear();
		k++;
	}
	printf("Ready with adding.\n");
}
