#ifndef KGGZ_LAUNCH_H
#define KGGZ_LAUNCH_H

#include <qwidget.h>

#include <qlabel.h>

class KGGZ_Launch : public QWidget
{
Q_OBJECT
public:
	KGGZ_Launch(QWidget *parent, char *name);
	~KGGZ_Launch();
protected slots:
	void accept();
	void setOpenSeats(int seats);
	void setReservedSeats(int seats);
	void setAISeats(int seats);
private:
	QLabel *count_open, *count_reserved, *count_ai;
};

#endif
