#ifndef GGZPLUGINPREFERENCES_H
#define GGZPLUGINPREFERENCES_H

#include <kcmodule.h>

class QLineEdit;

class GGZPluginPreferences : public KCModule
{
	Q_OBJECT
public:
	GGZPluginPreferences(QWidget *parent = 0, const char *name = 0, const QStringList& args = QStringList());

	void save();
	void load();

private:
	QLineEdit *m_server;
};

#endif

