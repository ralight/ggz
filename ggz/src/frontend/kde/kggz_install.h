#ifndef KGGZ_INSTALL_H
#define KGGZ_INSTALL_H

#include <qwidget.h>
#include <qmultilineedit.h>

class KGGZ_Install : public QWidget
{
Q_OBJECT
public:
	KGGZ_Install(QWidget *parent, char *name);
	~KGGZ_Install();
protected slots:
	void slotClose();
private:
	QMultiLineEdit *m_output;
};

#endif
