#ifndef DLG_AGAIN_H
#define DLG_AGAIN_H

#include <qwidget.h>

class DlgAgain : public QWidget
{
	Q_OBJECT
	public:
		DlgAgain(QWidget *parent, char *name);
		~DlgAgain();

		void setResult(const char *result);

	signals:
		void signalAgain();
		
	protected:
		void paintEvent(QPaintEvent *e);

	private:
		QString m_result;
};

#endif
