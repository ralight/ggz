#ifndef KGGZ_UPDATE_H
#define KGGZ_UPDATE_H

#include <qwidget.h>
#ifdef USE_FTP
#include <qurloperator.h>
#include <qpushbutton.h>
#include <qlistview.h>
#include <qprogressbar.h>
#include <qlabel.h>
#include "kggz_install.h"
#else
#define QNetworkOperation void // ouch :-)
#define QUrlOperator void
#define QPushButton void
#endif

class KGGZ_Update : public QWidget
{
Q_OBJECT
public:
	KGGZ_Update(QWidget *parent, char *name);
	~KGGZ_Update();
protected slots:
	void slotDownload();
	void slotShowfiles();
	void slotLStart(QNetworkOperation *op);
	void slotLFinished(QNetworkOperation *op);
	void slotLProgress(int done, int total, QNetworkOperation *op);
	void slotRStart(QNetworkOperation *op);
	void slotRFinished(QNetworkOperation *op);
	void slotRProgress(int done, int total, QNetworkOperation *op);
	void slotRConnectionStateChanged(int state, const QString &data);
	void slotRChildren(const QValueList<QUrlInfo> &children);
#ifdef USE_FTP
private:
	enum UpdateModes {UPDATE_SHOWFILES, UPDATE_DOWNLOAD};

	int prepare(UpdateModes mode);
	void ftpserver();
	void reset();

	QUrlOperator lop, rop;
	QPushButton *download, *showfiles;
	char *m_description;
	char *m_ftpserver;
	char *m_ftpdirectory;
	QListViewItem *m_selected;
	QLabel *m_display;
	QProgressBar *m_progress;
	KGGZ_Install *m_install;
#endif
};

#endif
