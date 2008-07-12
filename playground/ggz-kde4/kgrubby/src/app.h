#ifndef KGRUBBY_APP_H
#define KGRUBBY_APP_H

#include <ktabwidget.h>
#include <QList>
#include <QLabel>
#include <QMap>

class QTreeWidget;
class QTreeWidgetItem;
class QLabel;
class QComboBox;
class QLineEdit;
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
		QTreeWidget *m_pluginlist;
		QMap<QTreeWidgetItem*, QString> m_plugins;
		QMap<QString, QString> m_plugindialogs;
		QComboBox *m_networktype, *m_profile;
		QPushButton *m_profile_remove, *m_plugin_configure;
		QLineEdit *m_name, *m_owner, *m_lang, *m_host, *m_autojoin;
		QLabel *m_namelabel, *m_ownerlabel, *m_networktypelabel;
		QLabel *m_langlabel, *m_hostlabel, *m_autojoinlabel;
		QString m_profilename;
		QMap<QString, QList<QStringList> > m_profiledata;
		bool m_profilemodified;
};

#endif

