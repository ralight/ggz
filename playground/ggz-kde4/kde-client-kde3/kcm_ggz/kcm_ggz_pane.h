#ifndef KCM_GGZ_PANE_H
#define KCM_GGZ_PANE_H

#include <qwidget.h>

class KCMGGZPane : public QWidget
{
	Q_OBJECT
	public:
		KCMGGZPane(QWidget *parent = NULL, const char *name = NULL):QWidget(parent, name){}
		~KCMGGZPane(){}
		virtual void load() = 0;
		virtual void save() = 0;
		virtual QString caption() = 0;

	signals:
		void signalChanged();
};

#endif

