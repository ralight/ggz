#ifndef GGZAP_H
#define GGZAP_H

#include <qwidget.h>
#include <qlabel.h>
#include <qevent.h>

class GGZapHandler;

class GGZap : public QWidget
{
	Q_OBJECT
	public:
		GGZap(QWidget *parent = NULL, const char *name = NULL);
		~GGZap();

		void setModule(const char *modulename);
		void setFrontend(const char *frontendtype);

		void launch();

	public slots:
		void slotState(int state);
		void slotLaunch(char *name, char *frontend);
		void slotCancel();

	protected:
		void timerEvent(QTimerEvent *e);

	private:
		void fat(QLabel *label);
		void unfat(QLabel *label);
		
		QLabel *m_connect, *m_login, *m_room, *m_wait, *m_start;
		GGZapHandler *m_handler;
};

#endif

