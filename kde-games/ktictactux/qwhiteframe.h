#ifndef Q_WHITEFRAME_H
#define Q_WHITEFRAME_H

#include <qframe.h>
#include <qevent.h>

class QWhiteFrame : public QFrame
{
Q_OBJECT
public:
	QWhiteFrame(QWidget *parent = NULL, char *name = NULL);
	~QWhiteFrame();
protected slots:
	void mousePressEvent(QMouseEvent *event);
signals:
	void signalSelected(QWidget *);
};

#endif
