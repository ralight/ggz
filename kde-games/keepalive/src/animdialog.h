#ifndef KEEPALIVE_ANIMDIALOG_H
#define KEEPALIVE_ANIMDIALOG_H

#include <qwidget.h>

class QCanvasPixmapArray;
class QFrame;
class QLabel;

class AnimDialog : public QWidget
{
	Q_OBJECT
	public:
		AnimDialog(QWidget *parent = NULL, const char *name = NULL);
		~AnimDialog();
		void setText(QString text);
	public slots:
		void setAnimation(QCanvasPixmapArray *a);
	protected:
		void timerEvent(QTimerEvent *e);
	private:
		QCanvasPixmapArray *m_a;
		QFrame *m_f;
		QLabel *m_label;
		int m_counter;
};

#endif

