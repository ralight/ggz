#ifndef KGRUBBY_APP_H
#define KGRUBBY_APP_H

#include <ktabwidget.h>
//Added by qt3to4:
#include <Q3ValueList>
#include <QLabel>
#include <QMap>

class K3ListView;
class QLabel;
class QComboBox;
class QLineEdit;
class Q3CheckListItem;
class QPushButton;

class App : public KTabWidget
{
	Q_OBJECT
	public:
		App(QWidget *parent = NULL);
		~App();

		void saveProfile();
		void loadProfile();

	signals:
		void signalChanged(bool changed);

	protected slots:
		void slotProfileNew();
		void slotProfileDelete();
		void slotProfileChange();
		void slotPluginConfigure();
		void slotPluginSelected();
		void slotModified();

	private:
		K3ListView *m_pluginlist;
		QMap<Q3CheckListItem*, QString> m_plugins;
		QMap<QString, QString> m_plugindialogs;
		QComboBox *m_networktype, *m_profile;
		QPushButton *m_profile_remove, *m_plugin_configure;
		QLineEdit *m_name, *m_owner, *m_lang, *m_host, *m_autojoin;
		QLabel *m_namelabel, *m_ownerlabel, *m_networktypelabel;
		QLabel *m_langlabel, *m_hostlabel, *m_autojoinlabel;
		QString m_profilename;
		QMap<QString, Q3ValueList<QStringList> > m_profiledata;
		bool m_profilemodified;
};

#endif

