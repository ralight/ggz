/*
 * KGGZ - The KDE client for the GNU Gaming Zone
 * (C) 2000 Josef Spillner - dr_maux@users.sourceforge.net
 * Homepage: http://ggz.sourceforge.net (GGZ)
 *           http://mindx.sourceforge.net (MindX)
 *
 */

/*
 * KGGZ_Profiles
 * A class to be inherited by others (like KGGZ_Hosts or KGGZ_Ftp),
 * which handles the use of ggzcore for configuration items.
 *
 */

#ifndef KGGZ_PROFILES_H
#define KGGZ_PROFILES_H

/* Qt includes */
#include <qwidget.h>
#include <qlistview.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qlist.h>
#include <qlayout.h>

class KGGZ_Profiles : public QWidget
{
Q_OBJECT
public:
	KGGZ_Profiles(QWidget *parent, char *name);
	~KGGZ_Profiles();
	void setSectionName(char *name);
	void addField(char *name, char *entry, char *def);
	void readEntries();
signals:
	void signalUpdate(int profile);
	void signalClosed();
protected slots:
	void slotAccept();
private slots:
	void slotRemoveProfile();
	void slotAddProfile();
	void slotClose();
private:
	QListView *profiles_list;
	// it's new, it's hot:
	QList<QLabel> m_labellist;
	QList<QLineEdit> m_inputlist;
	QList<QString> m_entrylist;
	QList<QString> m_deflist;
	QString m_sectionname;
	QHBoxLayout *m_listbox;
};

#endif
