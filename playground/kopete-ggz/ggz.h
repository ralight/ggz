#ifndef GGZPLUGIN_H
#define GGZPLUGIN_H

#include <kxmlguiclient.h>

#include <qobject.h>
#include <qmap.h>

#include "kopetemessage.h"
#include "kopeteplugin.h"

class QStringList;
class QComboBox;

class GGZPluginGui : public QObject, public KXMLGUIClient
{
	Q_OBJECT
public:
	GGZPluginGui(Kopete::ChatSession *parent, const char *name = 0);

private slots:
	void slotGGZ(QString game);
	void slotGGZSelect();

private:
	Kopete::ChatSession *session;
};

class GGZPlugin : public Kopete::Plugin
{
	Q_OBJECT
public:
	GGZPlugin(QObject *parent, const char *name, const QStringList &args);
	~GGZPlugin();

private slots:
	void slotNewKMM(Kopete::ChatSession *);
	void slotProcessDisplay(Kopete::Message& msg);

private:
	QString m_self;
};

class GGZPluginLauncher
{
public:
	GGZPluginLauncher(QObject *parent);

	void launch(QString player1, QString player2, QString game);

private:
	QObject *m_parent;
};

class GGZPluginSelect : public QWidget
{
	Q_OBJECT
public:
	GGZPluginSelect(QWidget *parent = 0, const char *name = 0);
public slots:
	void slotSelected();
signals:
	void signalSelected(QString game);
private:
	QComboBox *m_gamescombo;
};

#endif

